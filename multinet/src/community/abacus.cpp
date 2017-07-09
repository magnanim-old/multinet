#include "community.h"
extern "C" {
#include <eclat.h>
}

#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <fstream>

namespace mlnet {

    void read_layers(ActorCommunitySharedPtr& com, FILE* file, const LayerListSharedPtr& layers) {
        bool found_separator=false;
        bool reading_number=false;
        int c;
        int lid; // layer identifier
        while (1) {
            c=getc(file);
            if (c=='\n' || c==EOF) {
                if (reading_number) com->add_layer(layers->get_at_index(lid));
                return;
            }
            
            if (!found_separator && c!=':') {
                continue;
            }
            else if (c==':') {
                found_separator=true;
                continue;
            }
            
            if (c>='0' && c<='9') {
                if (!reading_number) lid=(int)(c-'0');
                else lid=lid*10+(int)(c-'0');
                reading_number=true;
            }
            else if (c==' ') {
                if (reading_number) com->add_layer(layers->get_at_index(lid));
                reading_number=false;
                found_separator=false;
            }
        }
    }
    
    int read_actors(ActorCommunitySharedPtr& com, FILE* tidfile, const ActorListSharedPtr& actors) {
        actor_id id;
        int c;
        bool reading_number=false;
        while (1) {
            c=getc(tidfile);
            if (c>='0' && c<='9') {
                if (!reading_number) id=(int)(c-'0');
                else id=id*10+(int)(c-'0');
                reading_number=true;
            }
            else if (c==' ' || c=='\n' || c==EOF) {
                if (reading_number) com->add_actor(actors->get_at_index(id-1));
                reading_number=false;
                if (c==EOF) return 0;
                if (c=='\n') return 1;
            }
        }

    }
    
    ActorCommunityStructureSharedPtr read_eclat_communities(const ActorListSharedPtr& actors, const LayerListSharedPtr& layers, FILE* file, FILE* tidfile) {
        ActorCommunityStructureSharedPtr communities = actor_community_structure::create();
        ActorCommunitySharedPtr current = actor_community::create();
        while (read_actors(current,tidfile,actors)) {
            read_layers(current,file,layers);
            if (current->get_actors().size()>0) {
                communities->add_community(current);
                current = actor_community::create();
            }
        }
        communities->add_community(current);
        return communities;
    }

    ActorCommunityStructureSharedPtr abacus(const MLNetworkSharedPtr& mnet, int min_actors, int min_layers) {
        hash_map<LayerSharedPtr,CommunityStructureSharedPtr> single_layer_communities;
        for (LayerSharedPtr layer: *mnet->get_layers()) {
            single_layer_communities[layer] = label_propagation_single(mnet,layer);
        }
        return eclat_merge(mnet, single_layer_communities, min_actors, min_layers);
    }
    
    ActorCommunityStructureSharedPtr eclat_merge(const MLNetworkSharedPtr& mnet, const hash_map<LayerSharedPtr,CommunityStructureSharedPtr>& single_layer_communities, int min_actors, int min_layers) {
        
        ActorListSharedPtr actors = mnet->get_actors();
        LayerListSharedPtr layers = mnet->get_layers();
        hash_map<int,vector<string> > transactions;
        
        for (auto pair: single_layer_communities) {
            string layer_str = to_string(mnet->get_layers()->get_index(pair.first));
            int community = 0;
            for (CommunitySharedPtr c: (pair.second)->get_communities()) {
                string community_str = to_string(community);
                for (NodeSharedPtr node: c->get_nodes()) {
                    transactions[actors->get_index(node->actor)].push_back(community_str + ":" + layer_str);
                }
                community++;
            }
        }
        
        FILE* f_inp = std::tmpfile();
        if (!f_inp)
            throw FileNotFoundException("Cannot open tmp file");
        FILE* f_out = std::tmpfile();
        if (!f_out)
            throw FileNotFoundException("Cannot open tmp file");
        FILE* f_tid = std::tmpfile();
        if (!f_tid)
            throw FileNotFoundException("Cannot open tmp file");
        
        int     k = 0;             /* result variable */
        CCHAR   *recseps = NULL;      /* record  separators */
        CCHAR   *fldseps = NULL;      /* field   separators */
        CCHAR   *blanks  = NULL;      /* blank   characters */
        CCHAR   *comment = NULL;      /* comment characters */
        CCHAR   *hdr     = "";        /* record header  for output */
        CCHAR   *sep     = " ";       /* item separator for output */
        CCHAR   *imp     = " <- ";    /* implication sign for ass. rules */
        CCHAR   *dflt    = " (%S)";   /* default format for check */
        CCHAR   *info    = dflt;      /* format for information output */
        int     target   = ISR_CLOSED;       /* target type (e.g. closed/maximal) */
        ITEM    zmax     = ITEM_MAX;  /* maximum rule/item set size */
        double  smax     = 100;       /* maximum support of an item set */
        double  conf     = 80;        /* minimum confidence (in percent) */
        int     eval     = 'x';       /* additional evaluation measure */
        int     agg      = 'x';       /* aggregation mode for eval. measure */
        double  thresh   = 10;        /* threshold for evaluation measure */
        ITEM    prune    = ITEM_MIN;  /* (min. size for) evaluation pruning */
        int     sort     = 2;         /* flag for item sorting and recoding */
        int     algo     = 'a';       /* variant of eclat algorithm */
        int     mode     = ECL_DEFAULT|ECL_PREFMT;   /* search mode */
        int     pack     = 16;        /* number of bit-packed items */
        int     cmfilt   = -1;        /* mode for closed/maximal filtering */
        int     mtar     = 0;         /* mode for transaction reading */
        int     scan     = 0;         /* flag for scanable item output */
        ITEM    m;                    /* number of items */
        TID     n;                    /* number of transactions */
        //SUPP    w;                    /* total transaction weight */

        TABREAD  *tread  = NULL; /* table/transaction reader */
        ITEMBASE *ibase  = NULL; /* item base */
        TABAG    *tabag  = NULL; /* transaction bag/multiset */
        ISREPORT *report = NULL; /* item set reporter */
        ECLAT    *eclat  = NULL; /* eclat miner object */
        
        for (size_t i=0; i<actors->size(); i++) {
            for (string item: transactions[i]) {
                fprintf(f_inp,"%s ",item.c_str());
            }
            fprintf(f_inp,"\n");
        }
        rewind(f_inp);
        
        if ((cmfilt >= 0) && (target & (ISR_CLOSED|ISR_MAXIMAL)))
            mode |= (cmfilt > 0) ? ECL_VERT : ECL_HORZ;
        mode |= ECL_TIDS;
        mode = (mode & ~ECL_FIM16)    /* add packed items to search mode */
        | ((pack <= 0) ? 0 : (pack < 16) ? pack : 16);
        mode |= ECL_VERBOSE|ECL_NOCLEAN;
        
        /* --- read item selection/appearance indicators --- */
        ibase = ib_create(0, 0);      /* create an item base */
        if (!ibase) std::cout << "e" << std::endl;   /* to manage the items */
        tread = trd_create();         /* create a transaction reader */
        if (!tread) std::cout << "e" << std::endl;   /* and configure the characters */
        trd_allchs(tread, recseps, fldseps, blanks, "", comment);

        /* --- read transaction database --- */
        tabag = tbg_create(ibase);    /* create a transaction bag */
        if (!tabag) std::cout << "e" << std::endl;   /* to store the transactions */
        if (trd_open(tread, f_inp, NULL) != 0)
            std::cout << "e" << std::endl;
        k = tbg_read(tabag, tread, mtar);
        if (k < 0) std::cout << "e" << std::endl;
        trd_delete(tread, 1);         /* read the transaction database, */
        tread = NULL;                 /* then delete the table reader */
        m = ib_cnt(ibase);            /* get the number of items, */
        n = tbg_cnt(tabag);           /* the number of transactions, */
        //w = tbg_wgt(tabag);           /* the total transaction weight */
        //if (w != (SUPP)n)
        //    ;
        if ((m <= 0) || (n <= 0))     /* check for at least one item */
            return actor_community_structure::create();
        
        /* --- find frequent item sets/association rules --- */
        eclat = eclat_create(target, -min_actors, smax, conf, min_layers, zmax,
                             eval, agg, thresh, algo, mode);
        if (!eclat) std::cout << "e" << std::endl;   /* create an eclat miner */
        k = eclat_data(eclat, tabag, 0, sort);
        if (k) std::cout << "e" << std::endl;            /* prepare data for eclat */
        report = isr_create(ibase);   /* create an item set reporter */
        if (!report) std::cout << "e" << std::endl;  /* and configure it */
        k = eclat_report(eclat, report);
        if (k) std::cout << "e" << std::endl;            /* prepare reporter for eclat */
        if (isr_setfmt(report, scan, hdr, sep, imp, info) != 0)
            std::cout << "e" << std::endl;            /* set the output format strings */
        k = isr_tidopen(report, f_tid, NULL);  /* open the file for */
        if (k) std::cout << "e" << std::endl;   /* transaction ids */
        k = isr_open(report, f_out, NULL);
        if (k) std::cout << "e" << std::endl;
        if (isr_setup(report) < 0)    /* open the item set file and */
            std::cout << "e" << std::endl;            /* set up the item set reporter */
        k = eclat_mine(eclat, prune, 0);
        if (k) std::cout << "e" << std::endl;              /* find frequent item sets */
        //if (stats)                    /* print item set statistics */
        //    isr_prstats(report, stdout, 0);
        isr_flush(report);
        isr_tidflush(report);
        
        rewind(report->file);
        rewind(report->tidfile);
        ActorCommunityStructureSharedPtr result = read_eclat_communities(actors, layers, report->file, report->tidfile);
        
        std::fclose(f_inp);
        std::fclose(f_out);
        std::fclose(f_tid);
        //if (isr_close   (report) != 0)/* close item set output file - not needed: temporary file */
        
        //if (isr_tidclose(report) != 0)/* close trans. id output file */
        
        //CLEANUP;
        if (eclat)  eclat_delete(eclat, 0);
        if (report) isr_delete(report, 0);
        if (tabag)  tbg_delete(tabag,  0);
        if (tread)  trd_delete(tread,  1);
        if (ibase)  ib_delete (ibase);
        
        return result;
    }
    
    
}

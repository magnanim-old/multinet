#include "community.h"
#include "community/abacus.h"
extern "C" {
#include <eclat.h>
}

#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <fstream>

namespace mlnet {

    CommunityStructureSharedPtr read_supporting_tids(const ActorListSharedPtr& actors, FILE* file) {
        CommunityStructureSharedPtr communities = community_structure::create();
        CommunitySharedPtr current = community::create();
        actor_id id;
        int c;
        bool new_number=true;
        while (1) {
            c=getc(file);
            if (c==EOF) {
                if (current->get_nodes().size()>0)
                    current = community::create();
                break;
            }
            else if (c==' ') {
                if (!new_number) std::cout << actors->get_at_index(id-1)->name; // add actor to community
                std::cout << " ";
                new_number=true;
            }
            else if (c=='\n') {
                communities->add_community(current);
                current = community::create();
                if (!new_number) std::cout << actors->get_at_index(id-1)->name << std::endl;
                new_number=true;
            }
            else if (c>='0' && c<='9') {
                if (new_number) id=(int)(c-'0');
                else id=id*10+(int)(c-'0');
                new_number=false;
            }
        }
        return communities;
    }

    
    CommunityStructureSharedPtr abacus(const MLNetworkSharedPtr& mnet, const vector<CommunityStructureSharedPtr>& single_layer_communities, int min_sup) {
        
        ActorListSharedPtr actors = mnet->get_actors();
        vector<vector<string> > transactions(actors->size());
        
        int layer = 0;
        for (CommunityStructureSharedPtr cs: single_layer_communities) {
            string layer_str = to_string(layer);
            int community = 0;
            for (CommunitySharedPtr c: cs->get_communities()) {
                string community_str = to_string(community);
                for (NodeSharedPtr node: c->get_nodes()) {
                    transactions[actors->get_index(node->actor)].push_back(community_str + ":" + layer_str);
                }
                community++;
            }
            layer++;
        }
        
        /*
         string transactions_f = "/Users/matteomagnani/piropiro"; //std::tmpnam(nullptr);
         // TODO not 100% safe
         std::ofstream myfile(transactions_f);
         /*for (vector<string> trans: transactions) {
         for (string item: trans) {
         myfile << item << " ";
         }
         myfile << std::endl;
         }
        myfile << "a b c\n\na b c\na b d\n";
        myfile.close();
        
        string transaction_ids_f = transactions_f + "_tr";
         */
        
        FILE* f_inp = std::tmpfile();
        if (!f_inp)
            ;
        FILE* f_out = std::tmpfile();
        if (!f_out)
            std::cout << "fout" << std::endl;
        FILE* f_tid = fopen("/Users/matteomagnani/piropiro.tr","w"); //std::tmpfile();
        if (!f_tid)
            std::cout << "ftid" << std::endl;
        
        int     k = 0;             /* loop variables, counters */
        char    *s;                   /* to traverse the options */
        CCHAR   *fn_inp  = "/Users/matteomagnani/piropiro";      /* name of the input  file */
        CCHAR   *fn_out  = "/Users/matteomagnani/piropiro.out";      /* name of the output file */
        CCHAR   *fn_sel  = NULL;      /* name of item selection file */
        CCHAR   *fn_tid  = "/Users/matteomagnani/piropiro.tr";      /* name of transaction ids file */
        CCHAR   *fn_psp  = NULL;      /* name of pattern spectrum file */
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
        ITEM    zmin     = 1;         /* minimum rule/item set size */
        ITEM    zmax     = ITEM_MAX;  /* maximum rule/item set size */
        double  smin     = 1;        /* minimum support of an item set */
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
        int     stats    = 0;         /* flag for item set statistics */
        //PATSPEC *psp;                 /* collected pattern spectrum */
        ITEM    m;                    /* number of items */
        TID     n;                    /* number of transactions */
        SUPP    w;                    /* total transaction weight */

        TABREAD  *tread  = NULL; /* table/transaction reader */
        ITEMBASE *ibase  = NULL; /* item base */
        TABAG    *tabag  = NULL; /* transaction bag/multiset */
        ISREPORT *report = NULL; /* item set reporter */
        //TABWRITE *twrite = NULL; /* table writer for pattern spectrum */
        ECLAT    *eclat  = NULL; /* eclat miner object */
        
        for (vector<string> trans: transactions) {
            for (string item: trans) {
                fprintf(f_inp,"%s ",item.c_str());
            }
            fprintf(f_inp,"\n");
        }
        rewind(f_inp);
        
        
        if ((cmfilt >= 0) && (target & (ISR_CLOSED|ISR_MAXIMAL)))
            mode |= (cmfilt > 0) ? ECL_VERT : ECL_HORZ;
        mode |= ECL_TIDS;           /* turn "-" into "" for consistency */
                                   /* set transaction identifier flag */
        mode = (mode & ~ECL_FIM16)    /* add packed items to search mode */
        | ((pack <= 0) ? 0 : (pack < 16) ? pack : 16);
        if (target & ISR_RULES)       /* if to find association rules, */
            fn_psp = NULL;              /* no pattern spectrum possible */
        if (info == dflt) {           /* if default info. format is used, */
            if (target != ISR_RULES)    /* set default according to target */
                info = (smin < 0) ? " (%a)"     : " (%S)";
            else info = (smin < 0) ? " (%b, %C)" : " (%X, %C)";
        }                             /* select absolute/relative support */
        mode |= ECL_VERBOSE|ECL_NOCLEAN;
        
        /* --- read item selection/appearance indicators --- */
        ibase = ib_create(0, 0);      /* create an item base */
        if (!ibase) std::cout << "e" << std::endl;   /* to manage the items */
        tread = trd_create();         /* create a transaction reader */
        if (!tread) std::cout << "e" << std::endl;   /* and configure the characters */
        trd_allchs(tread, recseps, fldseps, blanks, "", comment);
        //if (fn_sel) {                 /* if an item selection is given */
        //    if (trd_open(tread, NULL, fn_sel) != 0)
        //        std::cout << "e" << std::endl;
        //    m = (target == ISR_RULES)   /* depending on the target type */
        //    ? ib_readapp(ibase,tread) /* read the item appearances */
        //    : ib_readsel(ibase,tread);/* or a simple item selection */
        //    if (m < 0) std::cout << "e" << std::endl;
        //    trd_close(tread);           /* close the input file */
        //}                             /* print a log message */
        
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
        w = tbg_wgt(tabag);           /* the total transaction weight */
        if (w != (SUPP)n)
            ;
        if ((m <= 0) || (n <= 0))     /* check for at least one item */
            std::cout << "e" << std::endl;           /* and at least one transaction */
        
        /* --- find frequent item sets/association rules --- */
        eclat = eclat_create(target, smin, smax, conf, zmin, zmax,
                             eval, agg, thresh, algo, mode);
        if (!eclat) std::cout << "e" << std::endl;   /* create an eclat miner */
        k = eclat_data(eclat, tabag, 0, sort);
        if (k) std::cout << "e" << std::endl;            /* prepare data for eclat */
        report = isr_create(ibase);   /* create an item set reporter */
        if (!report) std::cout << "e" << std::endl;  /* and configure it */
        k = eclat_report(eclat, report);
        if (k) std::cout << "e" << std::endl;            /* prepare reporter for eclat */
        //if (setbdr(report, w, zmin, &border, bdrcnt) != 0)
        //    std::cout << "e" << std::endl;             /* set the support border (if any) */
        //if (fn_psp && (isr_addpsp(report, NULL) < 0))
        //    std::cout << "e" << std::endl;            /* set a pattern spectrum if req. */
        if (isr_setfmt(report, scan, hdr, sep, imp, info) != 0)
            std::cout << "e" << std::endl;            /* set the output format strings */
        k = isr_tidopen(report, NULL, fn_tid);  /* open the file for */
        if (k) std::cout << "e" << std::endl;   /* transaction ids */
        k = isr_open(report, f_out, NULL);
        if (k) std::cout << "e" << std::endl;
        if (isr_setup(report) < 0)    /* open the item set file and */
            std::cout << "e" << std::endl;            /* set up the item set reporter */
        k = eclat_mine(eclat, prune, 0);
        if (k) std::cout << "e" << std::endl;              /* find frequent item sets */
        //if (stats)                    /* print item set statistics */
        //    isr_prstats(report, stdout, 0);
        isr_tidflush(report);
        
        rewind(report->tidfile);
        read_supporting_tids(actors, report->tidfile);
        
        if (isr_close   (report) != 0)/* close item set output file */
            std::cout << "e" << std::endl;

        if (isr_tidclose(report) != 0)/* close trans. id output file */
            std::cout << errno << std::endl;
        
        //CLEANUP;
        if (eclat)  eclat_delete(eclat, 0);
        if (report) isr_delete(report, 0);
        if (tabag)  tbg_delete(tabag,  0);
        if (tread)  trd_delete(tread,  1);
        if (ibase)  ib_delete (ibase);
        //if (border) free(border);
        //remove(community_file_name.data());
        
        return community_structure::create();
    }
    
    
}

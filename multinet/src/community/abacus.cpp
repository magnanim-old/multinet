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

    CommunityStructureSharedPtr abacus(const MLNetworkSharedPtr& mnet) {
        
        // temporary file where to store community info
        std::string community_file_name = "/Users/matteomagnani/piropiro";
        
        std::ofstream myfile;
        myfile.open(community_file_name);
        myfile << "a b c\na b c\na b d\n";
        myfile.close();
        
        static TABREAD  *mytread  = NULL; /* table/transaction reader */
        static ITEMBASE *myibase  = NULL; /* item base */
        static TABAG    *mytabag  = NULL; /* transaction bag/multiset */
        static ISREPORT *report = NULL; /* item set reporter */
        static double   *border = NULL; /* support border for filtering */
        static ECLAT    *eclat  = NULL; /* eclat miner object */
        int     mtar     = 0;
        
        
        std::cout << "prr" << std::endl;
        /* --- read transaction database --- */
        myibase = ib_create(0, 0);
        if (!myibase)
            ;//
        
        mytread = trd_create();         /* create a transaction reader */
        if (!mytread)
            ;   /* and configure the characters */
        //trd_allchs(tread, recseps, fldseps, blanks, "", comment);
        
        mytabag = tbg_create(myibase);
        if (!mytabag)
            ;//
        
        if (trd_open(mytread, NULL, community_file_name.data()) != 0)
            ;//trd_name(tread)
        
        int k = tbg_read(mytabag, mytread, mtar);
        if (k < 0) ;//error(-k, tbg_errmsg(tabag, NULL, 0));
        trd_delete(mytread, 1);         /* read the transaction database, */
        mytread = NULL;                 /* then delete the table reader */
        
        
        
        int m = ib_cnt(myibase);            /* get the number of items, */
        int n = tbg_cnt(mytabag);           /* the number of transactions, */
        std::cout << m << " " << n << std::endl;
        if ((m <= 0) || (n <= 0))     /* check for at least one item */
            ;                         /* and at least one transaction */

        
        /* --- find frequent item sets/association rules --- */
        // target
        // min support (%)
        // max support (%)
        // min confidence (%)
        // min itemset size
        // max itemset size
        // additional evaluation function
        // aggregation mode for eval. measure
        // threshold for eval. measure
        // variant of the algorithm
        // search mode
        ECLAT* abacus_eclat = eclat_create(ISR_CLOSED, 10, 100, 0, 1, ITEM_MAX,
                             'x', 'x', 10, 'i', ECL_DEFAULT|ECL_PREFMT);
        if (!eclat)
            ;
        k = eclat_data(eclat, tabag, 0, sort);
        if (k)
            ;              /* prepare data for eclat */
        report = isr_create(ibase);   /* create an item set reporter */
        if (!report)
            ;  /* and configure it */
        k = eclat_report(eclat, report);
        if (k)
            ;              /* prepare reporter for eclat */
        
        k = isr_tidopen(report, NULL, fn_tid);  /* open the file for */
        if (k)
            ;   /* transaction ids */
        //k = isr_open(report, NULL, fn_out);
        //if (k) error(k, isr_name(report));
        //if (isr_setup(report) < 0)    /* open the item set file and */
        //    error(E_NOMEM);             /* set up the item set reporter */
        k = eclat_mine(eclat, prune, 0);
        if (k)
            ;              /* find frequent item sets */
        if (isr_tidclose(report) != 0)/* close trans. id output file */
            error(E_FWRITE, isr_tidname(report));

        //CLEANUP;
        if (abacus_eclat)  eclat_delete(abacus_eclat, 0);
        if (report) isr_delete(report, 0);
        if (mytabag)  tbg_delete(mytabag,  0);
        if (mytread)  trd_delete(mytread,  1);
        if (myibase)  ib_delete (myibase);
        if (border) free(border);
        remove(community_file_name.data());
        
        return community_structure::create();
        

    }
    
}

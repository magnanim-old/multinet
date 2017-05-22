provider multinet_probes {
	probe lart_ac_start();
	probe lart_ac_end();
	probe lart_walk_start();
	probe lart_walk_end();
	probe lart_start(char* dataset, int t, char* eps, char* gamma);
	probe lart_end();

	probe glouvain_start(char* dataset, char* method, char* gamma, char* omega);
	probe glouvain_pass(int size, int rows, int cols, char* Q);
	probe glouvain_pass_end();
	probe glouvain_first_phase_start();
	probe glouvain_first_phase_end(int communities, char* dstep, char* dtot);
	probe glouvain_end(int communities, int rows, int cols, char* Q);

	probe pmm_start(char* dataset, int k, int ell);
	probe pmm_features_end();
	probe pmm_error(char* reason);
	probe pmm_svd_start();
	probe pmm_svd_end();
	probe pmm_kmeans_start();
	probe pmm_end();

};

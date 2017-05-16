provider multinet_probes {

	probe lart_ac_start();
	probe lart_ac_end();

	probe lart_walk_start();
	probe lart_walk_end();
	probe lart_pdistance_start(int X, int Y);

	probe lart_end(char* dataset, int t, char* eps, char* gamma, size_t clusters);

	probe glouvain_end(char* dataset, char* method, char* gamma, char* omega, size_t clusters);
	probe pmm_end(char* dataset, int k, int ell, size_t clusters);

};

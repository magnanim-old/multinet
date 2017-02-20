provider ml {

  /**
   * Fired when starting to convert MLnet to Adjacency Matrix
   */
  probe ml2am__reserve(size_t M, size_t R);
  /**
   * Fired when MLnet to Adjacency Matrix conversion is finished
   * Returns matrix dimensions
   */
  probe ml2am__end(size_t L, size_t M);

  /**
   * Fired when reserving memory for diagonal block matrix
   * Returns matrix dimensions
   */
  probe block__diag__reserve(size_t M, size_t R);

  probe diaga__reserve(size_t M, size_t R);

  probe dmat__newp__start();
  probe dmat__newp__end();

  probe pdistance__start(int X, int Y);
  probe pdistance__end();

  probe agglo__end(int clusters);

  probe agglo__prune__start();
  probe agglo__prune__end();

  probe find__dist__start();
  probe find__dist__end(int x, int y, char* val);

};

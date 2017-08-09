\name{Mining: communities}
\alias{Mining: communities}
\alias{abacus.ml}
\alias{clique.percolation.ml}
\alias{glouvain.ml}
\alias{lart.ml}
\alias{get.community.list.ml}
\title{
Community detection algorithms
}
\description{
Various algorithms to compute communities in multiplex networks, based on frequent itemset mining (abacus), adjacent cliques (clique percolation), modularity optimization (generalized louvain) and random walks (lart). get.community.list.ml is a commodity function translating the result of these algorithms into a list of node identifiers, and is internally used by the plotting function.}
\usage{
abacus.ml(mlnetwork, min.actors=3, min.layers=1)
clique.percolation.ml(mlnetwork, k=3, m=1)
glouvain.ml(mlnetwork, gamma=1, omega=1, limit=0)
lart.ml(mlnetwork, t=-1, eps=1, gamma=1)

get.community.list.ml(comm.struct, mlnetwork)
}
\arguments{
\item{mlnetwork}{A multilayer network.}
\item{min.actors}{Minimum number of actors to form a community.}
\item{min.layers}{Minimum number of times two actors must be in the same single-layer community to be considered in the same multi-layer community.}
\item{k}{Minimum number of actors in a clique. Must be at least 3.}
\item{m}{Minimum number of common layers in a clique.}
\item{gamma}{Resolution parameter for modularity in the generalized louvain and lart methods.}
\item{omega}{Inter-layer weight parameter in the generalized louvain method.}
\item{limit}{Limit parameter in the generalized louvain method: if the number of nodes exceeds this limit, then the modularity is computed on the fly without keeping the full data in memory.}
\item{t}{t parameter in the lart method. If a negative value is provided, then the number of layers times three is used.}
\item{eps}{Eps parameter in the lart method.}
\item{comm.struct}{The result of a community detection method.}
}
\value{
All community detection algorithms return a data frame where each row contains actor name, layer name and community identifier.

\code{get.community.list.ml} transforms the output of a community detection function into a list by grouping all the nodes having the same community identifier and the same layer.
}
\references{
Berlingerio, Michele, Pinelli, Fabio, and Calabrese, Francesco (2013). ABACUS: frequent pAttern mining-BAsed Community discovery in mUltidimensional networkS. Data Mining and Knowledge Discovery, 27(3), 294–320. (for abacus.ml())
Afsarmanesh, Nazanin, and Magnani, Matteo (2016). Finding overlapping communities in multiplex networks. Retrieved  arXiv 1602.03746 (for clique.percolation.ml())
Mucha, Peter J., Richardson, Thomas, Macon, Kevin, Porter, Mason A., and Onnela, Jukka-Pekka (2010). Community structure in time-dependent, multiscale, and multiplex networks. Science (New York, N.Y.), 328(5980), 876–8. Data Analysis, Statistics and Probability; Physics and Society. (for glouvain.ml())
Kuncheva, Zhana, and Montana, Giovanni (2015). Community detection in multiplex networks using locally adaptive random walks. Physics and Society; Machine Learning. (for lart.ml())
}
\seealso{\link{Plotting}}
\examples{
net <- ml.florentine()
abacus.ml(net)
clique.percolation.ml(net)
glouvain.ml(net)
lart.ml(net)
}

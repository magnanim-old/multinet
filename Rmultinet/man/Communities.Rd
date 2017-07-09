\name{Mining: communities}
\alias{Mining: communities}
\alias{clique.percolation.ml}
\alias{get.groups.ml}
\title{
Community detection algorithms
}
\description{
Extension of the clique percolation method, computing groups of adjacency-connected cliques. This method is based on the extraction of cliques, which can be very time-consuming. While the execution time depends on the topology of the input network, in general this method
is appropriate for not-too-large networks.
}
\usage{
clique.percolation.ml(mlnetwork, k=3, m1=1, m2=1, m3=1)

get.groups.ml(communities)
}
\arguments{
\item{mlnetwork}{An object of type MLNetwork.}
\item{k}{Minimum number of actors in a clique. Must be at least 3.}
\item{m1}{Minimum number of layers in a clique.}
\item{m2}{Minimum number of common layers in two adjacent cliques.}
\item{m3}{Minimum number of common layers in a community.}
\item{communities}{The result of a community detection method.}
}
\value{
\code{clique.percolation.ml} returns a matrix where each row contains actor name, layer name and community identifier.

\code{get.groups.ml} returns a list of vectors, one for each community, indicating the actors in the corresponding community. That is, it transforms the output of a community detection function by grouping all the actors having the same community identifier.
}
\references{
Afsarmanesh, Nazanin, and Matteo Magnani. 2016. "Finding Overlapping Communities in Multiplex Networks," http://arxiv.org/abs/1602.03746.
}
\examples{
net <- ml.aucs()
%clique.percolation.ml(net,4)
}

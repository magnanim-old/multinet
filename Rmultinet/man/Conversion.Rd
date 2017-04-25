\name{Conversion}
\alias{Conversion}
\alias{as.igraph.Rcpp_RMLNetwork}
\title{
Conversion to a simple or multi graph
}
\description{
Constructs a single graph resulting from merging one or more layers of the network and converts it into an iGraph object.
}
\usage{
\method{as.igraph}{Rcpp_RMLNetwork}(x, layers = NULL, merge.actors=TRUE, all.actors=FALSE, \dots)
}
\arguments{
\item{x}{An object of type MLNetwork.}
\item{layers}{A vector of names of layers. If NULL, all layers are included in the result.}
\item{merge.actors}{Whether the nodes corresponding to each actor should be merged into a single node (true) or kept separated (false).}
\item{all.actors}{Whether all actors in the multilayer network should be included in the output file (true) or only those present in at least one of the input layers (false).}
\item{\dots}{Additional arguments. None currently.}
}
\value{
An object of class iGraph.
}
\examples{
net <- ml.aucs()
multigraph <- as.igraph(net)
facebook <- as.igraph(net, "facebook")
}

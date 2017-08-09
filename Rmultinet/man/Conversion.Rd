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
\item{x}{A multilayer network.}
\item{layers}{A vector of names of layers. If NULL, all layers are included in the result.}
\item{merge.actors}{Whether the nodes corresponding to each actor should be merged into a single node (true) or kept separated (false).}
\item{all.actors}{Whether all actors in the multilayer network should be included in the result (true) or only those present in at least one of the input layers (false).}
\item{\dots}{Additional arguments. None currently.}
}
\value{
An object of class iGraph.
}
\examples{
net <- ml.aucs()
# using the default merge.actors=TRUE we create a multigraph,
# where each actor corresponds to a node in the result
multigraph <- as.igraph(net)
# this is a simple graph corresponding to the facebook layer
facebook1 <- as.igraph(net, "facebook")
# this includes also the actors without a facebook account
facebook2 <- as.igraph(net, "facebook", all.actors=TRUE)
# two layers are converted to an igraph object, where two
# nodes are used for each actor: one corresponding to the
# node on facebook, one to the node on lunch
f_l_net <- as.igraph(net, c("facebook","lunch"),
    merge.actors=FALSE)
}

\name{Measures: basic}
\alias{Measures: basic}
\alias{degree.ml}
\alias{degree.deviation.ml}
\alias{neighborhood.ml}
\alias{xneighborhood.ml}
\alias{connective.redundancy.ml}
\alias{relevance.ml}
\alias{xrelevance.ml}
\title{
Network analysis measures
}
\description{
These functions compute network analysis measures providing a basic description of the actors in the network.
}
\usage{
degree.ml(mlnetwork,actors=character(0),layers=character(0),mode="all")
degree.deviation.ml(mlnetwork,actors=character(0),
  layers=character(0),mode="all")
neighborhood.ml(mlnetwork,actors=character(0),layers=character(0),mode="all")
xneighborhood.ml(mlnetwork,actors=character(0),layers=character(0),mode="all")
connective.redundancy.ml(mlnetwork,actors=character(0),
  layers=character(0),mode="all")
relevance.ml(mlnetwork,actors=character(0),layers=character(0),mode="all")
xrelevance.ml(mlnetwork,actors=character(0),layers=character(0),mode="all")
}
\arguments{
\item{mlnetwork}{A multilayer network}
\item{actors}{An array of names of actors.}
\item{layers}{An array of names of layers.}
\item{mode}{This argument can take values "in", "out" or "all" to count respectively incoming edges, outgoing edges or both.}
}
\value{
\code{degree.ml} returns the number of edges adjacent to the input actor restricted to the specified layers.
\code{degree.deviation.ml} returns the standard deviation of the degree of an actor on the input layers. An actor with the same
degree on all layers will have deviation 0, while an actor with a lot of neighbors on one layer and only a few on another will have
a high degree deviation, showing an uneven usage of the layers (or layers with different densities).

\code{neighborhood.ml} returns the number of actors adjacent to the input actor restricted to the specified layers. \code{xneighborhood.ml} returns the number of actors adjacent to the input actor restricted to the specified layers and not present in the other layers.

\code{connective.redundancy.ml} returns 1 minus neighborhood/degree.

\code{relevance.ml} returns the percentage of neighbors present on the specified layers, while \code{xrelevance.ml} returns the percentage of neighbors present on the specified layers and not on others.
}
\references{
Berlingerio, Michele, Michele Coscia, Fosca Giannotti, Anna Monreale, and Dino Pedreschi. 2011. "Foundations of Multidimensional Network Analysis." In International Conference on Social Network Analysis and Mining (ASONAM), 485-89. IEEE Computer Society.

Magnani, Matteo, and Luca Rossi. 2011. "The ML-Model for Multi-Layer Social Networks." In International conference on Social Network Analysis and Mining (ASONAM), 5-12. IEEE Computer Society.
}
\examples{
net <- ml.aucs()
# degrees of all actors, considering edges on all layers
degree.ml(net)
# degree of actors U54 and U3, only considering layers work and coauthor
degree.ml(net,c("U54","U3"),c("work","coauthor"),"in")
# co-workers of U54
neighborhood.ml(net,"U54","work")
# co-workers of U54 who are not connected to U54 on other layers
xneighborhood.ml(net,"U54","work")
# percentage of neighbors of U54 who are also co-workers
relevance.ml(net,"U54","work")
# percentage of neighbors of U54 who would no longer
# be neighbors by removing this layer
xrelevance.ml(net,"U54","work")
}
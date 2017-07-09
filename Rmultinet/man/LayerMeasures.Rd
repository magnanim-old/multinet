\name{Measures: layer comparison}
\alias{Measures: layer comparison}
\alias{similarity.ml}
\alias{correlation.ml}
\title{
Network analysis measures
}
\description{
These functions can be used to compare different layers.
}
\usage{
similarity.ml(mlnetwork,layer1,layer2,method="jaccard.edges")
correlation.ml(mlnetwork,layer1,layer2,method="pearson.degree",mode="all")
}
\arguments{
\item{mlnetwork}{A multilayer network.}
\item{layer1}{The name of a layer.}
\item{layer2}{The name of a layer.}
\item{method}{This argument can take values: "jaccard.actors", "jaccard.edges", "jaccard.triangles", "coverage.actors", "coverage.edges", "sm.actors", "sm.edges", "rr.actors", "rr.edges", "kulczynski2.actors", and "kulczynski2.edges" for similarity.ml. The first part of the value indicates the type of comparison function (Jaccard, Coverage, Simple Matching, Russell Rao, Kulczynski), the second part indicates the configurations to which the comparison function is applied.

This argument can take values: "pearson.degree" and "rho.degree" for correlation.ml.}
\item{mode}{This argument is used for correlation.ml and can take values "in", "out" or "all" to consider respectively incoming edges, outgoing edges or both.}
}
\value{
A value between 0 and 1 (for similarity) or -1 and 1 (for correlation).
}
\examples{
net <- ml.aucs()
# returning the number of common edges divided by the union of all
# edges on the facebook and lunch layers
similarity.ml(net,"facebook","lunch")
# returning the percentage of actors in the kunch layer that are 
# also present in the facebook layer
similarity.ml(net,"facebook","lunch",method="coverage.actors")
# statistical degree correlation
correlation.ml(net,"facebook","lunch",method="pearson.degree")
}
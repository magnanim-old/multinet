\name{Plotting}
\alias{Plotting}
\alias{plot.Rcpp_RMLNetwork}
\title{
Drawing a multilayer network
}
\description{
This function uses the plotting functionality in igraph to draw multilayer networks.
}
\usage{
\S3method{plot}{Rcpp_RMLNetwork}(x, method="multi", layers=NULL, layer.colors=NULL,
  grid=NULL, layout=NULL, layout.par=list(), layout.independent=TRUE,
  communities=NULL, ...)
}
\arguments{
\item{x}{A multilayer network.}
\item{layers}{A vector of names of layers. If NULL, all layers are included in the result.}
\item{layer.colors}{A vector of colors, one for each layer. If not provided, this is set to rainbow(length(layers))}
\item{method}{Either "multi", to draw the multilayer network as a multigraph, or "slice", where each layer is plotted separately.}
\item{grid}{A vector of size 2 indicating the number of rows and columns where to draw the slides, in case \code{method="slice"} is specified.}
\item{layout}{Any layout used by igraph::plot. If this is not specified, \code{layout.fruchterman.reingold} is used.}
\item{layout.par}{If a \code{layout} function is specified, parameters for it should be specified using this parameter.}
\item{layout.independent}{If \code{method="slice"} is used, setting this parameter to TRUE results in drawing each layer independently of each other. Otherwise, a common layout is used, and nodes corresponding to the same actor are plotted in the same position in different slices.}
\item{communities}{The result of a community detection method. The corresponding actors will be marked in the plot.}
\item{...}{Additional parameters used by igraph::plot can be passed, e.g., vertex size, etc.}
}
\seealso{
\link{Mining: communities}
}
\examples{
net <- ml.florentine()
\dontrun{
plot(net,vertex.label.cex=.7)
plot(net,method="slice",vertex.label.cex=.7)
com <- clique.percolation.ml(net)
plot(net,method="slice",vertex.label.cex=.7,communities=com)}
}

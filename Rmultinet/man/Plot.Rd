\name{Plotting}
\alias{Plotting}
\alias{plot.Rcpp_RMLNetwork}
\alias{plot.ml}
\title{
Drawing a multilayer network
}
\description{
This function draws a multilayer network.
}
\usage{
\S3method{plot}{Rcpp_RMLNetwork}(x,
layout=NULL, grid=NULL, mai=.1,
vertex.shape=16, vertex.cex=1, vertex.color=NULL,
vertex.labels=NULL, vertex.labels.pos=3, vertex.labels.offset=.5, vertex.labels.cex=1,
edge.type=1, edge.width=1, edge.color=1,
edge.arrow.length=0.1, edge.arrow.angle=20,
com=NULL, com.cex=1, ...)
}
\arguments{
\item{x}{A multilayer network.}
\item{layout}{A data frame indicating the position of nodes. If NULL, the function layout.multiforce.ml is used to compute it.}
\item{grid}{A vector of size 2 indicating the number of rows and columns where to draw the layers.}
\item{mai}{Percentage of each frame reserved as internal margin. This only concerns nodes: text labels can be printed inside the margin or even outside the frame depending on their offset.}

\item{vertex.shape}{Symbol to use for nodes, corresponding to the parameter {pch} of the R \code{points} function.  This can either
be a single character or an integer code for one of a set of
graphics symbols.  See \code{?points} for more details.}
\item{vertex.cex}{Numeric *c*haracter *ex*pansion factor; multiplied by
\code{par("cex")} yields the final node size.}
\item{vertex.color}{Color of the vertexes. If NULL, all vertexes in the same layer are plotted using the same color.}

\item{vertex.labels}{A character vector or expression specifying the text to be
written besides each node. It corresponds to the parameter \code{labels} of the R \code{text} function.}
\item{vertex.labels.pos}{A position specifier for the text. Values of '1', '2', '3' and
'4', respectively indicate positions below, to the left of,
above and to the right of the specified coordinates. It corresponds to the parameter \code{pos} of the R \code{text} function.}
\item{vertex.labels.offset}{When \code{vertex.labels.pos} is specified, this value gives the offset of the label from the specified coordinate in fractions of a
character width. It corresponds to the parameter \code{offset} of the R \code{text} function.}
\item{vertex.labels.cex}{Numeric *c*haracter *ex*pansion factor; multiplied by
'par("cex")' yields the final character size.  'NULL' and
'NA' are equivalent to '1.0'. It corresponds to the parameter \code{cex} of the R \code{text} function.}

\item{edge.type}{Edge line type, corresponding to the 'lty' parameter of the R par function.  Line types can either be specified as an
integer (0=blank, 1=solid (default), 2=dashed, 3=dotted,
4=dotdash, 5=longdash, 6=twodash) or as one of the character
strings '"blank"', '"solid"', '"dashed"', '"dotted"',
'"dotdash"', '"longdash"', or '"twodash"', where '"blank"'
uses 'invisible lines' (i.e., does not draw them). See \code{?par} for more details. It accepts a vector of values which are recycled.}
\item{edge.width}{Edge line width, corresponding to the 'lwd' parameter of the R 'par' function. See \code{?par} for more details. It accepts a vector of
values which are recycled.}
\item{edge.color}{Color of the edges.}

\item{edge.arrow.length}{Length of the edges of the arrow head (in inches) - corresponding to the parameter of the R \code{arrows} function with the same name.}
\item{edge.arrow.angle}{Angle from the shaft of the arrow to the edge of the arrow
head - corresponding to the parameter of the R \code{arrows} function with the same name.}


\item{com}{The result of a community detection algorithm. When this parameter is set, a colored area is added behind each community.}
\item{com.cex}{Increases (>1) or decreases (<1) the margin around the nodes when the colored areas are drawn around the communities.}

\item{...}{Other graphical parameters.}
}
\seealso{
\link{Mining: communities}
}
\examples{
net <- ml.florentine()
\dontrun{
plot(net)
c <- clique.percolation.ml(net)
plot(net,vertex.labels.cex=.5,com=c)
}
net <- ml.aucs()
\dontrun{
plot(net,vertex.labels=NA)
title("AUCS network")
}
}

\name{Measures: layer comparison}
\alias{Measures: layer comparison}
\alias{layer.summary.ml}
\alias{layer.comparison.ml}
\title{
Network analysis measures
}
\description{
These functions can be used to compare different layers.
}
\usage{
layer.summary.ml(mlnetwork,layer,method="entropy.degree",mode="all")
layer.comparison.ml(mlnetwork,layers=character(0),method="jaccard.edges",mode="all",K=0)
}
\arguments{
\item{mlnetwork}{A multilayer network.}
\item{layer}{The name of a layer.}
\item{layers}{Names of the layers to be compared. If not specified, all layers are used.}
\item{method}{This argument can take several values. For layer summary:
"min.degree", "max.degree", "sum.degree", "mean.degree", "sd.degree", "skewness.degree", "kurtosis.degree", "entropy.degree", "CV.degree", "jarque.bera.degree".
For layer comparison:
\itemize{
\item{Overlapping:}{"jaccard.actors", "jaccard.edges", "jaccard.triangles", "coverage.actors", "coverage.edges",  "coverage.triangle","sm.actors", "sm.edges", "sm.triangles", "rr.actors", "rr.edges", "rr.triangles", "kulczynski2.actors", "kulczynski2.edges", "kulczynski2.triangles", "hamann.actors", "hamann.edges", "hamann.triangles". The first part of the value indicates the type of comparison function (Jaccard, Coverage, Simple Matching, Russell Rao, Kulczynski, Hamann), the second part indicates the configurations to which the comparison function is applied.}
\item{Distribution dissimilarity:}{"dissimilarity.degree", "KL.degree", "jeffrey.degree". Notice that these are dissimilarity functions: 0 means highest similarity}
\item{Correlation:}{"pearson.degree" and "rho.degree"}
}
}
\item{mode}{This argument is used for distribution dissimilarities and correlations (that is, those methods based on node degree) and can take values "in", "out" or "all" to consider respectively incoming edges, outgoing edges or both.}
\item{K}{This argument is used for distribution dissimilarity measures and indicates the number of histogram bars used to compute the divergence. If 0 is specified, then a "typical" value is used, close to the logarithm of the number of actors.}
}
\value{
A data frame with layer-by-layer coparisons. For each pair of layers, the data frame contains a value between 0 and 1 (for overlapping and distribution dissimilarity) or -1 and 1 (for correlation).
}
\examples{
net <- ml.aucs()

# computing similarity between layer summaries
s1 = layer.summary.ml(net,"facebook",method="entropy.degree")
s2 = layer.summary.ml(net,"lunch",method="entropy.degree")
relative.difference=abs(s1-s2)*2/(abs(s1)+abs(s2))
# other layer summaries
layer.summary.ml(net,"facebook",method="min.degree")
layer.summary.ml(net,"facebook",method="max.degree")
layer.summary.ml(net,"facebook",method="sum.degree")
layer.summary.ml(net,"facebook",method="mean.degree")
layer.summary.ml(net,"facebook",method="sd.degree")
layer.summary.ml(net,"facebook",method="skewness.degree")
layer.summary.ml(net,"facebook",method="kurtosis.degree")
layer.summary.ml(net,"facebook",method="entropy.degree")
layer.summary.ml(net,"facebook",method="CV.degree")
layer.summary.ml(net,"facebook",method="jarque.bera.degree")

# returning the number of common edges divided by the union of all
# edges for all pairs of layers (jaccard.edges)
layer.comparison.ml(net)
# returning the number of common edges divided by the union of all
# edges only for "lunch" and "facebook" (jaccard.edges)
layer.comparison.ml(net,layers=c("lunch","facebook"))
# returning the percentage of actors in the lunch layer that are
# also present in the facebook layer
layer.comparison.ml(net,method="coverage.actors")
# all overlapping-based measures:
layer.comparison.ml(net,method="jaccard.actors")
layer.comparison.ml(net,method="jaccard.edges")
layer.comparison.ml(net,method="jaccard.triangles")
layer.comparison.ml(net,method="coverage.actors")
layer.comparison.ml(net,method="coverage.edges")
layer.comparison.ml(net,method="coverage.triangles")
layer.comparison.ml(net,method="sm.actors")
layer.comparison.ml(net,method="sm.edges")
layer.comparison.ml(net,method="sm.triangles")
layer.comparison.ml(net,method="rr.actors")
layer.comparison.ml(net,method="rr.edges")
layer.comparison.ml(net,method="rr.triangles")
layer.comparison.ml(net,method="kulczynski2.actors")
layer.comparison.ml(net,method="kulczynski2.edges")
layer.comparison.ml(net,method="kulczynski2.triangles")
layer.comparison.ml(net,method="hamann.actors")
layer.comparison.ml(net,method="hamann.edges")
layer.comparison.ml(net,method="hamann.triangles")

# comparison of degree distributions (divergences)
layer.comparison.ml(net,method="dissimilarity.degree")
layer.comparison.ml(net,method="KL.degree")
layer.comparison.ml(net,method="jeffrey.degree")

# statistical degree correlation
layer.comparison.ml(net,method="pearson.degree")
layer.comparison.ml(net,method="rho.degree")
}

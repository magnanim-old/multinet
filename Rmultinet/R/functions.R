loadModule("multinet",TRUE)

# Functions loading some famous small datasets
# More datasets can be downloaded from the book webpage: multilayer.it.uu.se

ml.aucs <- function() {
	read.ml(system.file("extdata", "aucs.mpx", package="multinet"),"AUCS")
}

ml.toy <- function() {
	read.ml(system.file("extdata", "book.mpx", package="multinet"),"toy")
}

ml.florentine <- function() {
	read.ml(system.file("extdata", "florentine_families.mpx", package="multinet"),"Florentine families")
}

# Casting of (a portion of) a multilayer network into an igraph (multi)graph. This is done by creating an intermediate graphml file and loading it as an igraph file

as.igraph.Rcpp_RMLNetwork <- function (x, layers=NULL, merge.actors=TRUE, all.actors=FALSE, ...) {
    if (is.null(layers)) {
        layers <- layers.ml(x)
    }
    temp_f <- tempfile()
    write.ml(x, temp_f, format = "graphml", layers, ",", merge.actors, all.actors)
    g <- read.graph(temp_f, format = "graphml")
    unlink(temp_f)
    g
}

# (Rudimentary) plotting function.

    
plot.Rcpp_RMLNetwork <- function(x,
layout=NULL, grid=NULL, mai=.1,
vertex.shape=16, vertex.cex=1, vertex.color=NULL,
vertex.labels=NULL, vertex.labels.pos=3, vertex.labels.offset=.5, vertex.labels.cex=1,
edge.type=1, edge.width=1, edge.color=1,
edge.arrow.length=0.1, edge.arrow.angle=20,
com=NULL, com.cex=1, ...) {
    
    num.cols = num.layers.ml(x)
    num.rows = 1
    if (!is.null(grid)) {
        if (!length(grid)==2) stop("argument grid must have two elements")
        num.rows = grid[1]
        num.cols = grid[2]
    }
    
    if (is.null(layout)) {
        layout <- layout.multiforce.ml(x)
    }
    
    x_coord <- function(xyz_coord) {
        xyz_coord$x+xyz_coord$z%%num.cols*width
    }
   
   y_coord <- function(xyz_coord) {
       xyz_coord$y+(num.rows-1-xyz_coord$z%/%num.cols)*height
   }
   
    #
    x.min = min(layout$x)
    y.min = min(layout$y)
    x.max = max(layout$x)
    y.max = max(layout$y)
    
    width = x.max-x.min + mai*(x.max-x.min);
    x.min = x.min - mai/2*(x.max-x.min)
    height = y.max-y.min + mai*(y.max-y.min);
    y.min = y.min - mai/2*(y.max-y.min)
    
plot(NA,type="n",xlim=c(x.min,x.min+width*num.cols),ylim=c(y.min,y.min+height*num.rows),xaxt="n",yaxt="n",bty="n",xlab="",ylab="")

    # draw grid
    segments((0:num.cols*width)+x.min,y.min,(0:num.cols*width)+x.min,y.min+height*num.rows)
    segments(x.min,(0:num.rows*height)+y.min,x.min+width*num.cols,(0:num.rows*height)+y.min)
    
    # draw communities
    if (!is.null(com) && nrow(com)>0) {
        num.com <- max(com$cid)+1
        palette = rainbow(num.com, alpha=.5)
        draw.areas <- function(d) {
            xc <- x_coord(layout[d$aid,])
            yc <- y_coord(layout[d$aid,])
            # add some margin around nodes
            os <- par()$cxy*com.cex
            xc <- c(xc+os[1]/2,xc+os[1]/2,xc-os[1]/2,xc-os[1]/2)
            yc <- c(yc+os[2]/2,yc-os[2]/2,yc+os[2]/2,yc-os[2]/2)
                
            extreme.points = chull(xc,yc)
        xspline(xc[extreme.points],yc[extreme.points],open=F,shape=1,border=NA,col=palette[d$cid+1])
        }
        c.list <- get.community.list.ml(com,x);
        lapply(c.list, draw.areas)
    }
    
    # draw edges
    e <- edges.idx.ml(x)
    draw_edge <- function(d) {
        if (d['dir']==0) {
        segments(x_coord(layout[d['from'],]),
        y_coord(layout[d['from'],]),x_coord(layout[d['to'],]),y_coord(layout[d['to'],]), lty=edge.type, lwd=edge.width, col=edge.color)
        }
        if (d['dir']==1) {
        arrows(x_coord(layout[d['from'],]),
        y_coord(layout[d['from'],]),x_coord(layout[d['to'],]),y_coord(layout[d['to'],]), length=edge.arrow.length, angle=edge.arrow.angle, lty=edge.type, lwd=edge.width, col=edge.color)
        }
    }
    apply(e,1,draw_edge)

# draw nodes

if (is.null(vertex.color)) vertex.color=layout$z+1
points(x_coord(layout),y_coord(layout),pch=vertex.shape,col=vertex.color,cex=vertex.cex)

# draw labels
    if (is.null(vertex.labels)) vertex.labels=layout$actor
text(x_coord(layout),y_coord(layout),labels=vertex.labels, pos=vertex.labels.pos, offset=vertex.labels.offset, cex=vertex.labels.cex)
}


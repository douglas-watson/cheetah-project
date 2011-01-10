# plot the results from our simulation
library("ggplot2")

# theme_set(theme_bw())
theme_update(
	plot.rgin = c(0,0)
)

data = read.table("kneelag-amp.dat", header=TRUE)
attach(data)

qplot(amplitudeForeKnee, amplitudeHindKnee, fill = distance, data = data, 
	geom = "tile", main = "Facetted by kneePhaseLag") + 
	facet_wrap(~ kneePhaseLag) + scale_x_continuous(expand=c(0,0)) +
	scale_y_continuous(expand=c(0,0))
ggsave("kneelag-amp-dist.pdf", width = 11, height = 10)

qplot(amplitudeForeKnee, amplitudeHindKnee, fill = distance/duration, data = data, geom = "tile", main = "Facetted by kneePhaseLag") + facet_wrap(~ kneePhaseLag)+ scale_x_continuous(expand=c(0,0)) + scale_y_continuous(expand=c(0,0))
ggsave("kneelagamp-speed.pdf", width = 11, height = 10)

qplot(amplitudeForeKnee, amplitudeHindKnee, fill = exp(stability), data = data, geom = "tile", main = "Facetted by kneePhaseLag") + facet_wrap(~ kneePhaseLag)+ scale_x_continuous(expand=c(0,0)) + scale_y_continuous(expand=c(0,0))
ggsave("kneelagamp-stab.pdf", width = 11, height = 10)

qplot(amplitudeForeKnee, amplitudeHindKnee, fill = duration, data = data, geom = "tile", main = "Facetted by kneePhaseLag") + facet_wrap(~ kneePhaseLag)+ scale_x_continuous(expand=c(0,0)) + scale_y_continuous(expand=c(0,0))
ggsave("kneelagamp-dur.pdf", width = 11, height = 10)

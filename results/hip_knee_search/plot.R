# plot the results from our simulation
library("ggplot2")
theme_set(theme_bw())

data = read.table("hip-knee.dat", header=TRUE)
attach(data)
data$speed = distance / duration

qplot(amplitudeForeKnee, amplitudeHindKnee, fill = distance, data = data, 
	geom = "tile", main = "Facetted by hip amplitude") + 
	facet_wrap(~ amplitudeForeHip) + 
	scale_x_continuous("Fore Knee Amplitude", expand=c(0,0)) + 
	scale_y_continuous("Hind Knee Amplitude", expand=c(0,0)) +
	scale_colour_gradient("Distance")
ggsave("hip-knee-dist.pdf", width = 11, height = 10)

qplot(amplitudeForeKnee, amplitudeHindKnee, fill = exp(stability), data = data, geom = "tile", main = "Facetted by hip amplitude") + facet_wrap(~ amplitudeForeHip) + scale_x_continuous(expand=c(0,0)) + scale_y_continuous(expand=c(0,0))
ggsave("hip-knee-stab.pdf", width = 11, height = 10)

qplot(amplitudeForeKnee, amplitudeHindKnee, fill = duration, data = data, 
	geom = "tile", main = "Facetted by hip amplitude") + 
	facet_wrap(~ amplitudeForeHip) + 
	scale_x_continuous("Fore Knee Amplitude", expand=c(0,0)) + 
	scale_y_continuous("Hind Knee Amplitude", expand=c(0,0)) +
	scale_colour_continuous("Duration")
ggsave("hip-knee-dur.pdf", width = 11, height = 10)

qplot(amplitudeForeKnee, amplitudeHindKnee, fill = speed, data = data, 
	geom = "tile", main = "Facetted by hip amplitude") + 
	facet_wrap(~ amplitudeForeHip) + 
	scale_x_continuous("Fore Knee Amplitude", expand=c(0,0)) + 
	scale_y_continuous("Hind Knee Amplitude", expand=c(0,0)) + 
	scale_colour_gradient("Speed", low="", high="")
ggsave("hip-knee-speed.pdf", width = 11, height = 10)

stable = subset(data, duration == 15)
max(data$speed)
qplot(speed, data = stable, geom = "histogram")

#########
# Map parameters to speed
#########
# max speed is 0.32. Let's bin speed up to 0.3, making ten bins.
bins = seq(0, 0.3, 0.03)


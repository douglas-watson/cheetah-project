#!/usr/bin/env R
# Read results from the Cheetah simulations and attempt to display them in 
# a meaningful way. 
#
# Douglas Watson
# 25th October 2010
#########

library("ggplot2")

data <- read.table('trial1.txt', header=TRUE)
attach(data)

# set all independant variables to factors. Dependant variables are distance,
# angle, and duration. In this dataset, all simulations ran for 10 seconds.
data$amplitudeForeHip <- factor(amplitudeForeHip)
data$amplitudeHindHip <- factor(amplitudeHindHip) # these two are the same.
data$offsetForeHip <- factor(offsetForeHip)
data$offsetHindHip <- factor(offsetHindHip)
data$amplitudeForeKnee <- factor(amplitudeForeKnee)
data$amplitudeHindKnee <- factor(amplitudeHindKnee)
data$offsetKnee <- factor(offsetKnee)
data$kneePhaseLag <- factor(kneePhaseLag)
data$frequency <- factor(frequency)

# Convert angle to a binary upright / fallen variable:
data$upright <- round(angle)

# The simulation stopped at the third hip amplitude value
summary(amplitudeForeHip)

# Limit hip amplitude to 0.275
hip275 <- data[amplitudeForeHip == 0.275,]

# First few plots
qplot(kneePhaseLag, distance, data = hip275, colour = offsetForeHip, 
	facets = amplitudeForeKnee ~ amplitudeHindKnee, alpha = 0.3) + 
 	geom_smooth(aes(group = offsetForeHip))

ggsave("hip275-dist_v_kneePhaseLag_f_ampForeKnee-ampHindKnee_c_offForeHip.pdf", 
 	width = 16, height = 10)

# restrict to optimal knee amplitude, and plot. All remaining variables appear
# on this plot.
optimalKnee <- hip275[hip275$amplitudeHindKnee == 0.5,]
optimalKnee <- optimalKnee[optimalKnee$amplitudeForeKnee == 0.5,]
qplot(kneePhaseLag, distance, data = optimalKnee, 
	facets = offsetForeHip ~ offsetHindHip)
ggsave("optimalKnee-dist_v_kneePhaseLag_f_offsetForeHip-offsetHindHip.pdf",
	width = 16, height = 10)

import scipy as sp
import matplotlib.pyplot as plt
from scipy.optimize import fsolve

#########################
#####   FUNCTIONS   #####
#########################
### Approximation error
def error(f, x, y):
  return sp.sum( (f(x)-y)**2 ) # -> Problem with type of nd.array for some reason...


#####################
#####   LOGIC   #####
#####################
### Import the data.
data = sp.genfromtxt("web_traffic.tsv", delimiter = "\t")


### Preprocessing...
x = data[:,0] # We split the columns for computational effeciency.
y = data[:,1]

x = x[ ~sp.isnan(y) ] # Clear out NaN values.
y = y[ ~sp.isnan(y) ]


### Choosing a model for approximation.
# Linear model.
# fp1, residuals, rank, sv, rcond = sp.polyfit(x, y, 1, full=True)
# print("Linear model parameters: %s" % fp1)
# f1 = sp.poly1d(fp1)
# er1 = sp.sum( (f1(x)-y)**2 )
# print("1st order (linear) model approximation error: " + str(er1))

# 2nd order model.
fp2 = sp.polyfit(x, y, 2)
f2 = sp.poly1d(fp2)
er2 = sp.sum( (f2(x)-y)**2 )
print("2nd order model approximation error: " + str(er2))

# 3rd order model.
# fp3 = sp.polyfit(x, y, 3)
# f3 = sp.poly1d(fp3)
# er3 = sp.sum( (f3(x)-y)**2 )
# print("2nd order model approximation error: " + str(er3))

# Two-part linear model.
# inflection = int(3.5 * 7 * 24) # We divide the data at 3.5 weeks.
# xa = x[:inflection]
# xb = x[inflection:]
# ya = y[:inflection]
# yb = y[inflection:]
# fpa = sp.polyfit(xa, ya, 1)
# fpb = sp.polyfit(xb, yb, 1)
# fa = sp.poly1d(fpa)
# fb = sp.poly1d(fpb)
# era = sp.sum( (fa(x)-y)**2 )
# erb = sp.sum( (fb(x)-y)**2 )
# print("Two-part linear model approximation error: " + str(era+erb))


### Plot the data.
# plt.scatter(x, y, s=3) # Draw the data.
# xs = sp.linspace(0, x[-1], 1000) # Generating the x-values for plotting the models.
# plt.plot(xs, f1(xs), "g-", linewidth=2) # Draw the 1st order approximation (linear model).
# plt.plot(xs, f2(xs), "r-", linewidth=2) # Draw the 2nd order approximation.
# plt.plot(xs, f3(xs), "y-", linewidth=2) # Draw the 3rd order approximation.
# plt.plot(xs, fa(xs), "black", linewidth=2) # Draw the 1st part of the two-part linear approximation.
# plt.plot(xs[inflection:], fb(xs[inflection:]), "black", linewidth=2) # Draw the 2nd part of the two-part linear approximation.
# plt.title("Web Traffic")
# plt.xlabel("time")
# plt.ylabel("hits/hour")
# plt.xticks([w*7*24 for w in range(10)], ['week %i' %w for w in range(10)])
# plt.legend( ["d=%i" % f1.order], loc="upper left" )
# plt.autoscale(tight=True)
# plt.grid()
# plt.show()


### Extrapolate from the 2nd order binomial (best match) the point the traffic will reach 100000.
target_traffic = 100000
print("f2")
print(f2)
print("f2 - 100000")
print(f2 - target_traffic)
reached_max = fsolve(f2-target_traffic, 800)/(7*24)
print("100000 hits/hour will expected at week %f" % reached_max[0])
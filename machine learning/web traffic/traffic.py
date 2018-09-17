import scipy as sp
import matplotlib.pyplot as plt

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
fp1, residuals, rank, sv, rcond = sp.polyfit(x, y, 1, full=True) # Linear model.
print("Linear model parameters: %s" % fp1)
f1 = sp.poly1d(fp1)
er1 = sp.sum( (f1(x)-y)**2 )
print("Linear model approximation error: " + str(er1))


### Plot the data.
plt.scatter(x, y, s=3) # Draw the data.
xs = sp.linspace(0, x[-1], 1000) # Generating the x-values for plotting the models.
plt.plot(xs, f1(xs), "g-", linewidth=2) # Draw the 1st order approximation (linear model).
plt.title("Web Traffic")
plt.xlabel("time")
plt.ylabel("hits/hour")
plt.xticks([w*7*24 for w in range(10)], ['week %i' %w for w in range(10)])
plt.legend( ["d=%i" % f1.order], loc="upper left" )
plt.autoscale(tight=True)
plt.grid()
plt.show()

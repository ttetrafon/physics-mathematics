from matplotlib import pyplot as plt
import scipy as sp
import csv

#########################
#####   FUNCTIONS   #####
#########################

### Check for NaN values in an array dataset.
def checkNaN(set):
  check_array = sp.isnan(set)
  if (True in check_array):
    return True
  else:
    return False

### Distance between two points in the dataset.
# The points coordinates must be presented as ndarrays.
def distance(p1, p2):
  return sp.sum((p1-p2)**2)


#####################
#####   LOGIC   #####
#####################

### Import the data
# area - perimenter - compactness - length - width - assymetry coefficient - kernel groove length - label
data = sp.genfromtxt("seeds.tsv", delimiter = "\t")
data = sp.delete(data, 7, 1)
# AR = data[:,0] # area
# PE = data[:,1] # perimeter
# CO = data[:,2] # compactness
# LE = data[:,3] # length
# WI = data[:,4] # width
# AS = data[:,5] # assymetry coefficient
# LG = data[:,6] # kernel groove length
LA = [] # labels
with open("seeds.tsv") as infile:
  reader = csv.reader(infile, delimiter="\t")
  for row in reader:
    LA.append(row[-1])


### Check for NaN values, to and cull the data if needed.
#for set in [AR, PE, CO, LE, WI, AS, LG]:
#  print(checkNaN(set))


### Chek the a nearest neighbor model calculating distance with all data columns.
# num = data.shape[0]
# correctCount = 0
# for i in range(0, num):
#   minimumIndex = 0
#   distances = []
#   for j in range(0, num):
#     if (i != j):
#       distances.append( distance(data[i,:], data[j,:]) ) # Calculate all distances.
#   minimumIndex = distances.index( min(distances) )
#   if (minimumIndex >= i):
#     minimumIndex = minimumIndex + 1
#   #print(i, " (" , LA[i] , ") at ", minimumIndex, " (", LA[minimumIndex], ")")
#   if (LA[i] == LA[minimumIndex]):
#     correctCount = correctCount + 1
#
# print(correctCount/num)
# The result is 0.904761, meaning that flower type can be identified ~90% of the time by its closest neighbor.


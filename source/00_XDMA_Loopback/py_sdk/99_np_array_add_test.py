
from example import _add_arrays_1d
import numpy as np

data1 = np.array([1, 3, 5, 7, 9], dtype=np.uint64)
data2 = np.array([2, 4, 6, 8, 10], dtype=np.uint64)
var1 = _add_arrays_1d(data1,data2)
print('var1=', var1)
print(var1.dtype)
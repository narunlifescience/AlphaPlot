"""
	Some classes and functions that are particularly useful when doing calculations in QtiPlot;
	maybe also outside of it.
"""

import math, qti

class value:
	"""
		Scalar variable with associated error estimate.
		
		This class provides an easy way to do calculations with values subject to (experimental) errors.
		It is particularly useful for getting a quick idea of how where the main sources of uncertainty
		are in your calculations and how good the result is.
		
		However, if you need to do accurate hypthesis tests or have a precise range of possible values
		at a given clearance level, e.g. for a publication, you will probably want to do an analytical
		analysis of your final calculation and its error propagation. At the very least, you should be
		aware of the following shortcomings of this class:
		
		- it does not handle covariances
		- it does not handle asymmetric error ranges
		- it depends exclusively on linearized operations;
		  large errors will be propagated incorrectly in divisions, powers and roots
		
		The standard constructor is
		value(val, sigma)
		where val is the actual value and sigma is the error estimate (standard deviation). You can also
		construct a value from a QtiPlot table, like this:
		value(table, valcol, errcol, row)
		where table is a qti.Table instance, valcol the column containing the actual value, errcol the column
		containing the error estimate and row, if you haven't guessed this, the row from which to read the values.
	"""
	def __init__(self,val,var=None,sigma=True,row=None):
		if val.__class__==qti.Table:
			self.val,self.var = val.cell(var,row),val.cell(sigma,row)**2
		elif hasattr(val, "inherits") and val.inherits("Fit"):
			self.val = val.results()[var-1]
			covy = val.covarianceMatrix("tmp9999")
			self.var = covy.cell(var, var)
			covy.close()
		elif var == None:
			self.val,self.var = val,val
		elif sigma:
			self.val,self.var = val,var**2
		else:
			self.val,self.var = val,abs(var)
	
	# numeric protocol
	def __abs__(self): return value(abs(self.val), self.var, False)
	def __neg__(self): return value(-self.val, self.var, False)
	def __add__(self,o):
		if o.__class__==value:
			return value(self.val+o.val, self.var+o.var, False)
		else:
			return value(self.val+float(o), self.var, False)
	def __radd__(self,o): return self.__add__(o)
	def __sub__(self,o):
		if o.__class__==value:
			return value(self.val-o.val,self.var+o.var, False)
		else:
			return value(self.val-float(o),self.var, False)
	def __rsub__(self,o): return -self.__sub__(o)
	def __mul__(self,o):
		if o.__class__==value:
			return value(self.val*o.val,self.var*o.val**2+o.var*self.val**2, False)
		else:
			return value(self.val*float(o),self.var*float(o)**2, False)
	def __rmul__(self,o): return self.__mul__(o)
	def __div__(self,o):
		if o.__class__==value:
			return value(self.val/o.val, self.var/o.val**2+o.var*self.val**2/o.val**4, False)
		else:
			return value(self.val/float(o), self.var/float(o)**2, False)
	def __rdiv__(self,o):
		return value(float(o)/self.val, self.var*float(o)**2/self.val**4, False)
	def __pow__(self,o):
		if o.__class__==value:
			return value(self.val**o.val, self.var*o.val**2*self.val**(2*(o.val-1))+o.var*math.ln(self.val)*self.val**o.val, False)
		else:
			return value(self.val**float(o), self.var*float(o)**2*self.val**(2*(float(o)-1)), False)
	def __rpow__(self,o):
		return value(float(o)**self.val, self.var*math.ln(o)*float(o)**self.val, False)
	
	# comparison protocol
	def __lt__(self,o):
		if o.__class__==value: return self.val < o.val
		else: return self.val < float(o)
	def __le__(self,o):
		if o.__class__==value: return self.val <= o.val
		else: return self.val <= float(o)
	def __eq__(self,o):
		if o.__class__==value: return self.val == o.val
		else: return self.val == float(o)
	def __ge__(self,o):
		if o.__class__==value: return self.val >= o.val
		else: return self.val >= float(o)
	def __gt__(self,o):
		if o.__class__==value: return self.val > o.val
		else: return self.val > float(o)
		
	# convenience functions
	def sigma(self):
		"compute the error estimate (standard deviation)"
		return math.sqrt(self.var)
	def __str__(self):
		return str(self.val) + " +/- " + str(self.sigma())
	def sqrt(self):
		"error estimate aware square root function"
		if self.val==0: return value(0, self.var, False)
		return value(math.sqrt(self.val), self.var/(4*self.val), False)

def cartesic(r, phi):
	"Convert polar coordinates to cartesic ones. Arguments have to instances of class value."
	x = r*value(math.cos(phi.val), (math.sin(phi.val))**2*phi.var, False)
	y = r*value(math.sin(phi.val), (math.cos(phi.val))**2*phi.var, False)
	return (x, y)

def polar(x, y):
	"Convert (2D) cartesic coordinates to polar ones. Arguments have to instances of class value."
	r = (x**2+y**2).sqrt()
	yx = y/x
	phi = value(math.atan2(y.val, x.val), yx.var/(1+yx.val**2)**2, False)
	return (r, phi)

def lookup(tab, col, txt):
	"""
	  lookup(tab, col, txt):
	  search column col of QtiPlot table tab for a cell with text content txt,
	  returning the row of the first match; None on failure.
	"""
	for i in range(1,tab.numRows()+1):
		if tab.text(col, i) == txt: return i

class vec4:
	"""
	  A relativistic 4-vector.
	  Computations assume a diag(1, -1, -1, -1) metric.
	  Components may be any numbers that can either be cast to float or provide a sqrt() method.
	"""
	def __init__(self, one, x=None, y=0, z=0):
		self.t, self.x, self.y, self.z = 0,0,0,0
		try: self.t, self.x, self.y, self.z = one[0], one[1], one[2], one[3]
		except IndexError: pass
		except (AttributeError, TypeError):
			try: self.t, self.x, self.y, self.z = one,x[0],x[1],x[2]
			except IndexError: self.t, self.x, self.y, self.z = one,x[0],x[1],0
			except (AttributeError,TypeError): self.t, self.x, self.y, self.z = one,x,y,z
	def __add__(self, o):
		return vec4(self.t+o.t, self.x+o.x, self.y+o.y, self.z+o.z)
	def __sub__(self, o):
		return vec4(self.t-o.t, self.x-o.x, self.y-o.y, self.z-o.z)
	def __mul__(self, o):
		return self.t*o.t-self.x*o.x-self.y*o.y-self.z*o.z
	def __str__(self):
		return "(%s; %s, %s, %s)" % (self.t, self.x, self.y, self.z)
	def __abs__(self):
		s = self*self
		if s < 0:
			if (hasattr(s, "sqrt")): return -(-s).sqrt()
			else: return -math.sqrt(-s)
		else:
			if hasattr(s, "sqrt"): return s.sqrt()
			else: return math.sqrt(s)
	def abs3(self):
		"compute the absolute value of the spatial part of this 4-vector"
		s = self.x**2+self.y**2+self.z**2
		if hasattr(s, "sqrt"): return s.sqrt()
		else: return math.sqrt(s)
	def gamma(self):
		"assuming this is an energy-momentum four-vector, return the corresponding gamma factor"
		return self.t/abs(self)
	def beta(self):
		"assuming this is an energy-momentum four-vector, return the corresponding beta factor"
		s = 1-(abs(self)/self.t)**2
		if hasattr(s, "sqrt"): return s.sqrt()
		else: return math.sqrt(s)
	def betagamma(self):
		"compute self.beta()*self.gamma() more efficiently"
		s = (self.t/abs(self))**2-1
		if hasattr(s, "sqrt"): return s.sqrt()
		else: return math.sqrt(s)
		
def exportTableToTeX(t, filename=None):
	"""
		exportToTeX(table, filename=None):
		Export table as TeX-tabular to filename. If filename==None, popup a file selection dialog.
	"""
	from PyQt4.QtGui import QFileDialog
	if not filename:
		filename=QFileDialog.getSaveFileName(qti.app,"QtiPlot - Export TeX table","","All files *;;TeX documents (*.tex *.TEX);;");
	f=open(filename,'w')
	f.write('\\begin{tabular}{|' + 'c|'*t.numCols() + '}\\hline\n')
	for col in range(1,t.numCols()):
		f.write(t.colName(col) + ' & ')
	f.write(t.colName(t.numCols()) + ' \\\\\\hline\n')
	for row in range(1,t.numRows()+1):
		val = False
		for col in range(1,t.numCols()+1):
			if t.text(col,row) != "": val = True
		if val:
			for col in range(1,t.numCols()):
				f.write(t.text(col,row) + ' & ')
			f.write(t.text(t.numCols(),row) + ' \\\\\n')
	f.write('\\hline\n\\end{tabular}\n')
	f.close()

def exportAllTablesToTeX(folder, dir, recurs=True):
	for w in folder.windows():
		if w.isA("Table"): exportTableToTeX(w, dir+w.name()+".tex")
	if recurs:
		for f in folder.folders():
			exportAllTablesToTeX(f, dir, True)


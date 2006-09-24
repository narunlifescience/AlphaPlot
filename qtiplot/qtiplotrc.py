import __main__

# import standard math functions and constants into global namespace
math = __import__("math")
for name in dir(math):
  f = getattr(math,name)
  setattr(__main__,name,f)
  # make functions available in QtiPlot's math function list
  if callable(f):
    qti.mathFunctions[name] = f

# import selected parts of scipy.special (if available) into global namespace
# See www.scipy.org for information on SciPy and how to get it.
try:
  scipy = __import__("scipy.special")
  special = getattr(scipy, "special")
  special_functions = [
    # Airy Functions
    "airy", "airye", "ai_zeros", "bi_zeros",
    # Elliptic Functions and Integrals
    "ellipj", "ellipk", "ellipkinc", "ellipe", "ellipeinc",
    # Bessel Functions
    "jn", "jv", "jve",
    "yn", "yv", "yve",
    "kn", "kv", "kve",
    "iv", "ive",
    "hankel1", "hankel1e", "hankel2", "hankel2e",
    "lmbda",
    "jnjnp_zeros", "jnyn_zeros",
    "jn_zeros", "jnp_zeros",
    "yn_zeros", "ynp_zeros",
    "y0_zeros", "y1_zeros", "y1p_zeros",
    "j0", "j1", "y0", "y1", "i0", "i0e", "i1", "i1e", "k0", "k0e", "k1", "k1e",
    # Integrals of Bessel Functions
    "itj0y0", "it2j0y0", "iti0k0", "it2i0k0", "besselpoly",
    # Derivatives of Bessel Functions
    "jvp", "yvp", "kvp", "ivp", "h1vp", "h2vp",
    # Spherical Bessel Functions
    "sph_jn", "sph_yn", "sph_jnyn", "sph_in", "sph_kn", "sph_inkn",
    # Ricatti-Bessel Functions
    "riccati_jn", "riccati_yn",
    # Struve Functions
    "struve", "modstruve", "itstruve0", "it2struve0", "itmodstruve0",
    # Gamma and Related Functions
    "gamma", "gammaln", "gammainc", "gammaincinv", "gammaincc", "gammainccinv",
    "beta", "betaln", "betainc", "betaincinv", "betaincinva", "betaincinvb",
    "psi", "rgamma", "polygamma",
    # Error Function and Fresnel Integrals
    "erf", "erfc", "erfinv", "erfcinv", "erf_zeros",
    "fresnel", "fresnel_zeros", "fresnelc_zeros", "fresnels_zeros", "modfresnelp", "modfresnelm",
    # Legendre Functions
    "lpn", "lqn", "lpmn", "lqmn", "lpmv", "sph_harm",
    # Orthogonal polynomials
    "legendre", "sh_legendre",
    "chebyt", "chebyu", "chebyc", "chebys", "sh_chebyt", "sh_chebyu",
    "jacobi", "sh_jacobi",
    "laguerre", "genlaguerre",
    "hermite", "hermitenorm",
    "gegenbauer",
    # HyperGeometric Functions
    "hyp2f1",
    "hyp1f1", "hyperu", "hyp0f1",
    "hyp2f0", "hyp1f2", "hyp3f0",
    # Parabolic Cylinder Functions
    "pbdv", "pbvv", "pbwa", "pbdv_seq", "pbvv_seq", "pbdn_seq",
    # mathieu and Related Functions (and derivatives)
    "mathieu_a", "mathieu_b", "mathieu_even_coeff", "mathieu_odd_coeff",
    "mathieu_cem", "mathieu_sem", "mathieu_modcem1", "mathieu_modcem2", "mathieu_modsem1", "mathieu_modsem2",
    # Spheroidal Wave Functions
    "pro_ang1", "pro_rad1", "pro_rad2",
    "obl_ang1", "obl_rad1", "obl_rad2",
    "pro_cv", "obl_cv", "pro_cv_seq", "obl_vc_seq",
    "pro_ang1_cv", "pro_rad1_cv", "pro_rad2_cv",
    "obl_ang1_cv", "obl_rad1_cv", "obl_rad2_cv",
    # Kelvin Functions
    "kelvin", "kelvin_zeros",
    "ber", "bei", "berp", "beip", "ker", "kei", "kerp", "keip",
    "ber_zeros", "bei_zeros", "berp_zeros", "beip_zeros", "ker_zeros", "kei_zeros", "kerp_zeros", "keip_zeros",
    # Other Special Functions
    "expn", "exp1", "expi",
    "wofz", "dawson",
    "shichi", "sici", "spence",
    "zeta", "zetac",
    # Convenience Functions
#    "cbrt", "exp10", "exp2",
#    "radian", "cosdg", "sindg", "tandg", "cotdg",
#    "log1p", "expm1", "cosm1",
    "round",
  ]
  for name in special_functions:
    try:
      f = getattr(special, name)
      setattr(__main__, name, f)
      # make functions available in QtiPlot's math function list
      if callable(f):
        qti.mathFunctions[name] = f
    except(AttributeError): pass
except(ImportError):
  print("Could not load scipy.special; special functions will not be available.")

# make Qt API available (it gets imported in any case by the qti module)
#global qt
#import qt

# import QtiPlot's classes to the global namespace (particularly useful for fits)
from qti import *

# import selected methods of ApplicationWindow into the global namespace
appImports = (
    "table", "newTable",
    "matrix", "newMatrix",
    "graph", #"newGraph",
    "note", "newNote",
    "plot",
    "activeFolder", "rootFolder"
    )
for name in appImports:
  setattr(__main__,name,getattr(qti.app,name))


import __main__

def import_to_global(modname, attrs=None, math=False):
	"""
		import_to_global(modname, (a,b,c,...), math): like "from modname import a,b,c,...",
		but imports to global namespace (__main__).
		If math==True, also registers functions with QtiPlot's math function list.
	"""
	mod = __import__(modname)
	for submod in modname.split(".")[1:]:
		mod = getattr(mod, submod)
	if attrs==None: attrs=dir(mod)
	for name in attrs:
		f = getattr(mod, name)
		setattr(__main__, name, f)
		# make functions available in QtiPlot's math function list
		if math and callable(f): qti.mathFunctions[name] = f

# Import standard math functions and constants into global namespace.
import_to_global("math", None, True)

# Import selected parts of scipy.special (if available) into global namespace.
# See www.scipy.org for information on SciPy and how to get it.
have_scipy = False
try:
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
		"beta", "betaln", "betainc", "betaincinv",
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
		# Mathieu and related Functions (and derivatives)
		"mathieu_a", "mathieu_b", "mathieu_even_coef", "mathieu_odd_coef",
		"mathieu_cem", "mathieu_sem", "mathieu_modcem1", "mathieu_modcem2", "mathieu_modsem1", "mathieu_modsem2",
		# Spheroidal Wave Functions
		"pro_ang1", "pro_rad1", "pro_rad2",
		"obl_ang1", "obl_rad1", "obl_rad2",
		"pro_cv", "obl_cv", "pro_cv_seq", "obl_cv_seq",
		"pro_ang1_cv", "pro_rad1_cv", "pro_rad2_cv",
		"obl_ang1_cv", "obl_rad1_cv", "obl_rad2_cv",
		# Kelvin Functions
		"kelvin", "kelvin_zeros",
		"ber", "bei", "berp", "beip", "ker", "kei", "kerp", "keip",
		"ber_zeros", "bei_zeros", "berp_zeros", "beip_zeros", "ker_zeros", "kei_zeros", "kerp_zeros", "keip_zeros",
		# Other Special Functions
		"expn", "exp1", "expi",
		"wofz", "dawsn",
		"shichi", "sici", "spence",
		"zeta", "zetac",
		# Convenience Functions
		"cbrt", "exp10", "exp2",
		"radian", "cosdg", "sindg", "tandg", "cotdg",
		"log1p", "expm1", "cosm1",
		"round",
	]
	import_to_global("scipy.special", special_functions, True)
	have_scipy = True
	print("Loaded %d special functions from scipy.special." % len(special_functions))
except(ImportError): pass
 
# Import selected parts of pygsl.sf (if available) into global namespace.
# See pygsl.sourceforge.net for information on pygsl and how to get it.
try:
	# special functions not defined in SciPy
	special_functions = [
			# Restriction functions
			"angle_restrict_pos_err", "angle_restrict_symm_err",
			# Gamma and Related Functions
			"choose", "fact", "doublefact",
			"gammastar", "gammainv",
			"lnfact", "lndoublefact",
			"poch", "pochrel", "lnpoch", "lnpoch_sgn",
			"psi_1_int", "psi_1piy", "psi_int", "psi_n",
			# Fermi-Dirac integrals
			"fermi_dirac_0", "fermi_dirac_half", "fermi_dirac_1",
			"fermi_dirac_3half", "fermi_dirac_2",
			"fermi_dirac_inc_0", "fermi_dirac_int", "fermi_dirac_m1",
			"fermi_dirac_mhalf",
			# Elliptic Functions and Integrals
			"ellint_D", "ellint_F",
			"ellint_RC", "ellint_RD", "ellint_RF", "ellint_RJ",
			"elljac",
			# Error Function and Fresnel Integrals
			"log_erfc",
			# Other Integrals
			"atanint",
			"expint_3", "expint_E1", "expint_E2", "expint_Ei",
			# Legendre Functions and Spherical Harmonics
			"conicalP_0", "conicalP_1", "conicalP_half", "conicalP_mhalf",
			"conicalP_sph_reg", "conicalP_cyl_reg",
			"legendre_H3d", "legendre_H3d_0", "legendre_H3d_1",
			"legendre_Q0", "legendre_Q1", "legendre_Ql",
			"legendre_sphPlm",
			# Coulomb Functions
			"hydrogenicR", "hydrogenicR_1",
			"coulomb_CL", "coulomb_wave_FG",
			# Lambert's W function
			"lambert_W0", "lambert_Wm1",
			# Synchrotron functions
			"synchrotron_1", "synchrotron_2",
			# Transport functions
			"transport_2", "transport_3", "transport_4", "transport_5",
			# Clausen function
			"clausen",
			# Coupling coefficients
			"coupling_3j", "coupling_6j", "coupling_9j",
			# Debye functions
			"debye_1", "debye_2", "debye_3", "debye_4",
			# Dilogarithm
			"dilog",
			# Zeta Functions
			"eta", "eta_int",
			"hzeta", "zeta_int",
			# HyperGeometric Functions
			"hyperg_1F1_int",
			"hyperg_2F1_conj", "hyperg_2F1_conj_renorm", "hyperg_2F1_renorm",
			"hyperg_U_e10", "hyperg_U_int", "hyperg_U_int_e10",
			# Trigonometric Functions
			"hypot", "sinc",
			"lnsinh", "lncosh",
			# Other Special Functions
			"log_abs", "taylorcoeff",
			]
	# special functions also defined in SciPy
	special_functions_doublets = [
			# Airy functions
			"airy_Ai", "airy_Ai_deriv", "airy_Ai_scaled", "airy_Ai_deriv_scaled",
			"airy_Bi", "airy_Bi_deriv", "airy_Bi_scaled", "airy_Bi_deriv_scaled",
			"airy_zero_Ai", "airy_zero_Ai_deriv", "airy_zero_Bi", "airy_zero_Bi_deriv",
			# Bessel functions
			"bessel_I0", "bessel_I1", "bessel_In", "bessel_Inu",
			"bessel_I0_scaled", "bessel_I1_scaled", "bessel_In_scaled", "bessel_Inu_scaled",
			"bessel_J0", "bessel_J1", "bessel_Jn", "bessel_Jnu",
			"bessel_K0", "bessel_K1", "bessel_Kn", "bessel_Knu",
			"bessel_K0_scaled", "bessel_K1_scaled", "bessel_Kn_scaled", "bessel_Knu_scaled",
			"bessel_Y0", "bessel_Y1", "bessel_Yn", "bessel_Ynu",
			"bessel_i0_scaled", "bessel_i1_scaled", "bessel_i2_scaled", "bessel_il_scaled",
			"bessel_j0", "bessel_j1", "bessel_j2", "bessel_jl",
			"bessel_k0_scaled", "bessel_k1_scaled", "bessel_k2_scaled", "bessel_kl_scaled",
			"bessel_y0", "bessel_y1", "bessel_y2", "bessel_yl",
			"bessel_lnKnu",
			"bessel_zero_J0", "bessel_zero_J1", "bessel_zero_Jnu",
			# Gamma and Related Functions
			"beta", "beta_inc",
			"gamma", "gamma_inc_P", "gamma_inc_Q",
			"lnbeta", "lngamma",
			"psi",
			# Elliptic Functions and Integrals
			"ellint_E", "ellint_Ecomp", "ellint_Kcomp", "ellint_P",
			# Error Function and Fresnel Integrals
			"erf", "erfc", "erf_Q", "erf_Z",
			# Gegenbauer polynomials
			"gegenpoly_1", "gegenpoly_2", "gegenpoly_3", "gegenpoly_n",
			# HyperGeometric Functions
			"hyperg_0F1", "hyperg_1F1", "hyperg_2F0", "hyperg_2F1",
			"hyperg_U",
			# Orthogonal Polynomials
			"laguerre_1", "laguerre_2", "laguerre_3", "laguerre_n",
			"legendre_P1", "legendre_P2", "legendre_P3",
			"legendre_Pl", "legendre_Plm",
			# Zeta Functions
			"zeta",
			# Other special functions
			"Shi", "Chi", "Si", "Ci",
			"dawson",
			"log_1plusx", "log_1plusx_mx",
			]
	import_to_global("pygsl.sf", special_functions, True)
	if have_scipy:
		print("Loaded %d special functions from pygsl.sf." % len(special_functions))
	else:
		import_to_global("pygsl.sf", special_functions_doublets, True)
		print("Loaded %d special functions from pygsl.sf." % (len(special_functions) + len(special_functions_doublets)))
except(ImportError): pass


# make Qt API available (it gets imported in any case by the qti module)
global QtGui
from PyQt4 import QtGui

# import QtiPlot's classes to the global namespace (particularly useful for fits)
from qti import *

# import selected methods of ApplicationWindow into the global namespace
appImports = (
	"table", "newTable",
	"matrix", "newMatrix",
	"graph", "newGraph",
	"note", "newNote",
	"plot",
	"activeFolder", "rootFolder"
	)
for name in appImports:
	setattr(__main__,name,getattr(qti.app,name))

# import utility module
import sys
sys.path.append(".")
import_to_global("qtiUtil")

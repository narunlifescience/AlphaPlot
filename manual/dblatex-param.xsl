<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>

<!-- 
     réduit la taille des images d'un facteur 0.4 pour le fichier PDF
	par rapport à la taille utilisée pour les fichiers html 
-->
<xsl:import href="qtiplot-style.xsl"/>

<xsl:param name="imagedata.default.scale">scale=0.4</xsl:param>

<xsl:param name="qandaset.default.label" value="number"></xsl:param>

<!--
The callouts referenced in a callout list are hot links if the parameter is set to 1. Then, the references are in red such like any other cross-reference link in the document.
-->
<xsl:param name="callout.linkends.hot" value="1"></xsl:param>

<!-- 
params to be passed to the hyperref.sty package:

We want TOC links in the titles (not in the page numbers), and blue. 
 -->
<xsl:param name="latex.hyperparam">colorlinks,linkcolor=blue</xsl:param>

</xsl:stylesheet>

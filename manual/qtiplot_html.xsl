<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>
<xsl:import href="file:////usr/share/xml/docbook/stylesheet/nwalsh/html/chunk.xsl"/>

	<!-- html.stylesheet
	The html.stylesheet parameter is either empty, indicating that no 
	stylesheet LINK tag should be generated in the HTML output, or 
	it is a list of one or more stylesheets.
	Multiple stylesheets are space-delimited. 
	If you need to reference a stylesheet URI that includes a space, 
	encode it with %20. A seprate HTML LINK element will be 
	generated for each stylesheet in the order they are listed in the 
	parameter.
	-->	
<xsl:param name="html.stylesheet">qtiplot.css</xsl:param>

	<!-- css.decoration
	If css.decoration is turned on, then HTML elements produced by 
	the stylesheet may be decorated with STYLE attributes. 
	For example, the LI tags produced for list items may include a 
	fragment of CSS in the STYLE attribute which sets the CSS 
	property "list-style-type".
	-->
<xsl:param name="css.decoration" select="1"></xsl:param>


	<!-- use.id.as.filename
	If use.id.as.filename is non-zero, the filename of chunk elements 
	that have IDs will be derived from the ID value.
	-->
<xsl:param name="use.id.as.filename" select="'1'"></xsl:param>
<xsl:param name="base.dir" select="'html/'"></xsl:param>

	<!-- generate.index
	demande la création d'un index global
	-->
<xsl:param name="generate.index" select="1"></xsl:param>
<xsl:param name="index.on.type" select="0"></xsl:param>

	<!-- 
		contrôle de la table des matières
	-->
<xsl:param name="generate.section.toc.level" select="1"></xsl:param>
<xsl:param name="toc.max.depth">2</xsl:param>
<xsl:param name="generate.toc">
appendix  toc,title
book      toc,title,figures
chapter   toc,title
</xsl:param>

	<!-- 
		contrôles divers
	-->
<xsl:param name="make.year.ranges" select="0"></xsl:param>

</xsl:stylesheet>

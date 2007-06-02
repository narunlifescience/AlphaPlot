<!--
	Style sheet for producing the HTML version of the Qtiplot manual using OpenJade
	(http://openjade.sourceforge.net) and the Docbook stylesheets from
	http://docbook.sourceforge.net/projects/dsssl
-->
<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [

<!-- Path to main Docbook->html stylesheet. You have to adapt this to your installation. -->
<!ENTITY docbook-html-stylesheet SYSTEM "/usr/share/sgml/docbook/stylesheet/dsssl/modular/html/docbook.dsl" CDATA DSSSL>
<!--ENTITY docbook-html-stylesheet SYSTEM "/usr/share/sgml/docbook/dsssl-stylesheets-1.79/html/docbook.dsl" CDATA DSSSL-->

<!-- Stylesheet function library for creating Windows HTML help and Qt Assistant profiles. -->
<!ENTITY docbook-help-stylesheet SYSTEM "./help.dsl" CDATA DSSSL>
]>

<style-sheet>
<style-specification use="docbook-html docbook-help">
<style-specification-body>

(define %html-ext% 
	;; extension of HTML output files
	".html")

(define %root-filename%
	;; name of main HTML file (without extension)
	"index")

(define %stylesheet%
	;; CSS stylesheet to be used in HTML pages
	"html/qtiplot.css")

(define %css-decoration%
	;; Active l'utilisation des CSS dans le code HTML g���
	;; Notament les param�res CLASS= des principales balises
	#t)

(define %body-attr% 
	;; Attribut utilis�dans la balise BODY
	;; Ici on laisse faire la feuille de style CSS
	(list))

(define use-output-dir
	;; D�init si le r�ertoire de destination %output-dir% doit �re utilis�pour les fichiers HTML
	#t)

(define %output-dir%
	;; directory to place all output files in (if not specified on the command line)
	"html")

(define %adp-filename%
 ;; File name of Qt Assistant profile (without extension).
  "qtiplot")

(define %htmlhelp-filename%
	;; Base name of HTML-Help output fiels (.chm, .hhp, .hhc and .hhk)
	"qtiplot")

;(define (chunk-skip-first-element-list)
;  ;; forces the Table of Contents on separate page
;  '())

(define %document-name%
	"QtiPlot_manual_en")
(define %document-title%
	"QtiPlot Help Browser")
(define %adp-icon%
	"./common/qtiplot_logo.png")
(define %adp-startpage%
	"./index.html")
(define %adp-aboutpage%
	"./index.html")
(define %adp-aboutmenutext%
	"Handbook")

;; Overrides the root node definition (taken from docbook.dsl, v1.76)
;; This is a hack to add generation of HTML help and Qt Assistant files.

(root
 (make sequence
   (process-children)
   (with-mode manifest
     (process-children))
   (if html-index
       (with-mode htmlindex
	 (process-children))
       (empty-sosofo))
;;   (make-htmlhelp-project)
   (make-qt-assistant-project)
	))
</style-specification-body>
</style-specification>
<external-specification id="docbook-html" document="docbook-html-stylesheet">
<external-specification id="docbook-help" document="docbook-help-stylesheet">
</style-sheet>

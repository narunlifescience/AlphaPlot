<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY dbstyle SYSTEM "/usr/share/sgml/docbook/stylesheet/dsssl/modular/html/docbook.dsl" CDATA DSSSL>
]>

<style-sheet>
<style-specification use="docbook">
<style-specification-body>

(define %html-ext% 
	;; Extention par d�aut des fichiers HTML
	".html"
)

(define %root-filename%
	;; Nom du .fichier HTML principal
	"manual-0.8.9"
)

(define %stylesheet%
	;; Nom et emplacement de la feuille de style (CSS) utilis� par les pages HTML
	"html/qtiplot.css"
)

(define %css-decoration%
	;; Active l'utilisation des CSS dans le code HTML g���
	;; Notament les param�res CLASS= des principales balises
	#t)

(define %body-attr% 
	;; Attribut utilis�dans la balise BODY
	;; Ici on laisse faire la feuille de style CSS
	(list
	)
)

(define use-output-dir
	;; D�init si le r�ertoire de destination %output-dir% doit �re utilis�pour les fichiers HTML
	#t
)

(define %output-dir%
	;; Si rien d'autre n'est d�ini sur la ligne de commande, on utilise le r�ertoire de destination suivant :
	"html"
 )

</style-specification-body>
</style-specification>
<external-specification id="docbook" document="dbstyle">
</style-sheet>

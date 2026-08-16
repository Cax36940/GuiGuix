(use-modules (guix packages)
             (gnu packages)
             (guix))

(define (clean-field field)
  (string-map
   (lambda (c)
     (cond
      ((char=? c #\newline) #\space)
      (else c)))
   (or field "")))

(define (package->string package)
  (string-append
   (clean-field (package-name package)) "\n"
   (clean-field (package-version package)) "\n"
   (clean-field (package-synopsis package)) "\n"
   (clean-field (package-description package)) "\n"))

(fold-packages
 (lambda (package result)
   (display (package->string package))
   result)
 '())
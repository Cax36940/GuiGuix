(use-modules (guix packages)
             (gnu packages)
             (guix))

(define (clean-field field)
  (if field
      (string-replace-substring field "\n" " ")
      ""))

(fold-packages
 (lambda (package result)
   (display (package-name package))
   (newline)

   (display (package-version package))
   (newline)

   (display (clean-field (package-synopsis package)))
   (newline)

   (display (clean-field (package-description package)))
   (newline)

   result)
 '())
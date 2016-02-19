* Fill PDF Forms in Drupal with PDFTk
Original work by @cmcintosh

* Dependencies: 
- Drupal 7.*
- Token Module
- EntityAPI Module
- Entityform Module
- PDFTk installed on same server as Drupal

* Description

PDF Forms enables PDF forms to be populated using token data from Drupal. It is based on the concept similar to entityforms: Each PDF Form template has its own bundle. When you want to create a populated version of the PDF template, you create a new entity in the PDF Form template's bundle - a "submission".   

* How to use

- Enable module. Visit /admin/config/media/pdf_forms and set path to PDFTk

- Visit /admin/content/pdf_forms , and create a new PDF form bundle. You must upload a valid PDF form file (i.e. http://help.adobe.com/en_US/Acrobat/9.0/Samples/interactiveform_enabled.pdf ). After you save the form it will be parsed for field names. Those filed names will then become the fields that store for each entity of the form's bundle

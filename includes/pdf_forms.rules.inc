<?php

/**
* Implementation of hook_rules_data_type_info
*/
function pdf_forms_rules_data_type_info() {
  $info = array();

  // Crate the data type pdf_form_data
  $info['pdf_form'] = array(
      'label' => t('pdf form submission'),
      'parent' => 'entity',
      'group' => t('PDF Form'),
  );

  return $info;
}

/**
* Implements hook_rules_event_info
*/
function pdf_forms_rules_event_info() {

  $info = array();
  
  $defaults = array(
    'group' => t('PDF Forms'),
  );

  $info['pdf_forms_delete'] = $defaults + array(
    'label' => t('After deleting a PDF Form submission'),
    'group' => t('PDF Forms'),
    'variables' => array(
      'pdf_form' => array(
        'type' => 'pdf_form',
      ),
      'pdf' => array(
        'type' => 'file',
        'label' => t('PDF File'),
      )
    )
  );

  $info['pdf_forms_insert'] = $defaults + array(
    'label' => t('After saving a new PDF Form submission'),
    'group' => t('PDF Forms'),
    'variables' => array(
      'pdf_form' => array(
        'type' => 'pdf_form',
        'label' => t('PDF Form Submission'),
      ),
      'pdf' => array(
        'type' => 'file',
        'label' => t('PDF File'),
      )
    )
  );

  $info['pdf_forms_update'] = $defaults + array(
    'label' => t('After updating a PDF Form submission'),
    'group' => t('PDF Forms'),
    'variables' => array(
      'pdf_form' => array(
        'type' => 'pdf_form',
        'label' => t('PDF Form Submission'),
      ),
      'pdf' => array(
        'type' => 'file',
        'label' => t('PDF File'),
      )
    )
  );

  $info['pdf_forms_presave'] = $defaults + array(
    'label' => t('Before saving a PDF Form submission'),
    'group' => t('PDF Forms'),
    'variables' => array(
      'pdf_form' => array(
        'type' => 'pdf_form',
        'label' => t('PDF Form Submission'),
      ),
      'pdf' => array(
        'type' => 'file',
        'label' => t('PDF File'),
      )
    )
  );

  $info['pdf_forms_view'] = $defaults + array(
    'label' => t('A PDF Form submission is viewed'),
    'group' => t('PDF Forms'),
    'variables' => array(
      'pdf_form' => array(
        'type' => 'pdf_form',
        'label' => t('PDF Form Submission'),
      ),
      'pdf' => array(
        'type' => 'file',
        'label' => t('PDF File'),
      )
    )
  );

  return $info;
}


/**
* Implements hook_rules_action_info
* - fillpdf_entity_save_file Generates a FillPDF file for a provided entity.
*/
function pdf_forms_rules_action_info() {

  $info = array();

  $defaults = array(
    'group' => t('PDF Forms'),
  );

   $info['pdf_forms_save_file'] = $defaults + array(
    'label' => t('Generate PDF File From Entity'),
    'base' => 'pdf_forms_rules_action_save_file',
    'provides' => array(
      'file' => array(
        'type' => 'file',
        'label' => t('PDF File')
      )
    ),
    'parameter' => array(
      'bundle' => array(
        'type' => 'text',
        'label' => t('PDF Form'),
        'description' => t("The machine name of the PDF Form template to populate"),
        'options list' => 'pdf_forms_bundle_options',
       // 'restriction' => 'input',
      ),
      'data_entities' => array(
        'type' => 'list<entity>',
        'label' => t('Entities'),
        'description' => t('These entities will be used to retrieve token values when populating the PDF form '),
        'optional' => TRUE,
        'allow null' => TRUE,
      ),
    )
  );

  return ($info);
}


/**
* Provides options for the Action to select from.
*/
function pdf_forms_bundle_options() {
 
  // @TODO: provide bundle filtering based on the entity types contained in the entity list
  //$template_fid = db_query('SELECT fid FROM {pdf_forms_type} WHERE bundle = :bundle', array(':bundle' => $entity->bundle))->fetchField();
  $templates = db_query('SELECT label, bundle FROM {pdf_forms_type}');

  $bundle_options = array();

  foreach($templates as $template) {
    $bundle_options[$template->bundle] = $template->label;
  }

  return $bundle_options;
}


/**
* File Selector
*/
function pdf_forms_file_options() {

}

/**
* Implements hook_rules_condition_info
* - Entity Type
* - Bundle Type
*/
function pdf_forms_rules_condition_info() {

}

/**
* - Generates a PDF file for the provided entity
*/
function pdf_forms_rules_action_save_file($bundle, $data_entities) {
  
  $entity = entity_get_controller('pdf_form')->create($bundle); 
  global $user;
  $entity->uid = $user->uid;

  module_load_include('inc', 'pdf_forms', 'includes/pdf_forms.pdftk');

  $file = pdftk_create_completed_form($entity, $data_entities);
  $entity->fid = $file->fid;

  return array('file' => $file);

}

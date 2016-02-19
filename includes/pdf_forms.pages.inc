<?php
/**
*
* @file
* Contains the functions related to generating the various pages for the module.
*/

/**
* - Generates the settings form for the module.
*
* @param $form - form element passed to generate the form.
* @param $form_state - passes the state values for the form.
*
* @return $form - returns the built form array for generating our admin form.
*/
function pdf_forms_admin_settings($form, &$form_state) {
  $form['pdf_forms_pdftk_path'] = array(
    '#type' => 'textfield',
    '#title' => t('PDFTK Path'),
    '#description' => t('Enter the path to the PDFTK Executable.'),
    '#default_value' => variable_get('pdf_forms_pdftk_path', 'pdftk')
  );

  $form['pdf_forms_template_path'] = array(
    '#type' => 'textfield',
    '#title' => t('PDF Form Path'),
    '#description' => t('Enter the path to store the PDF Form Templates.'),
    '#default_value' => variable_get('pdf_forms_template_path', 'public://pdf_form_templates')
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings')
  );

  return $form;
}

/**
* Form validation
*/
function pdf_forms_admin_settings_validate($form, &$form_state) {

  // Check if the path is correct.
  module_load_include('inc', 'pdf_forms', 'includes/pdf_forms.pdftk');
  if (!pdf_forms_pdftk_check($form_state['values']['pdf_forms_pdftk_path'])) {
    form_set_error('pdf_forms_pdftk_path', t('Invalid path for PDFTK executable, or invalid permissions.'));
  }

  if(!file_prepare_directory($form_state['values']['pdf_forms_template_path'])) {
    form_set_error('pdf_forms_template_path', t('Could not create directory for saving PDF Form Templates.'));
  }

}

/**
* Form submit handler.
*/
function pdf_forms_admin_settings_submit($form, &$form_state) {
  variable_set('pdf_forms_pdftk_path', $form_state['values']['pdf_forms_pdftk_path']);
  file_prepare_directory($form_state['values']['pdf_forms_template_path']);
  variable_set('pdf_forms_template_path', $form_state['values']['pdf_forms_template_path']);
}

/**
* Overview display for the Admin page (Pdf Forms)
*/
function pdf_form_type_admin_overview() {
  $header = array(
    array(
      'field' => 'label',
      'data' => t('Title')
    ),
    array(
      'field' => 'description',
      'data' => t('Description')
    ),
    array(
      'field' => 'status',
      'data' => t('Status')
    ),
    array(
      'data' => t('Total Submissions')
    ),
    array(
      'data' => t('Actions')
    )
  );
  $query = db_select('pdf_forms_type', 't');
  $query->fields('t', array('bundle', 'label', 'description', 'status'));
  $query->extend('PagerDefault')->limit(20);

  $rows = array();
  $results = $query->execute();
  foreach($results as $row) {
    
    $submission_total = db_query('SELECT COUNT(*) FROM {pdf_forms_data} WHERE bundle = :bundle', array(':bundle' => $row->bundle))->fetchField();
    $action_links =  l('edit form', "admin/content/pdf_forms/{$row->bundle}/edit")
      . " | " . l('view submissions', "pdf_form/{$row->bundle}/submissions")
      . " | " . l('submission link', "pdf_form/{$row->bundle}");
    $rows[] = array(
      $row->label,
      $row->description,
      $row->status == 0 ? t('unpublished') : t('published'),
      $submission_total,
      $action_links
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows, 'empty' => t('No form types created yet.'))) . theme('pager');
}

/**
* Overview of submissions for forms.
*/
function pdf_forms_submissions_overview() {
  $header = array(
    array(
      'field' => 't.label',
      'data' => t('Form')
    ),
    array(
      'field' => 'u.name',
      'data' => t('Author')
    ),
    array(
      'field' => 's.created',
      'data' => t('Created')
    ),
    array(
      'field' => 's.fid',
      'data' => t('View Form')
    )
  );

  $query = db_select('pdf_forms_data', 's');
  $query->fields('s', array('submission_id', 'uid', 'created', 'fid'))
    ->join('users', 'u', 'u.uid = s.uid');
  $query->fields('u', array('name'))
    ->join('pdf_forms_type', 't', 't.bundle = s.bundle');
  $query->fields('t', array('label'));
  $query->extend('TableSort')->orderByHeader($header);
  $query->extend('PagerDefault')->limit(20);
  $results = $query->execute();
  $rows = array();

  foreach($results as $row) {
    $rows[] = array(
      $row->label,
      $row->name,
      format_date($row->created),
      l('view submission', "pdf_form_submission/{$row->submission_id}/view"),
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows, 'empty' => t('No submissions in database.'))) . theme('pager');
}

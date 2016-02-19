<?php

/**
* This callback function loads all the defined fields for a PDF page.
*/
function pdf_forms_sigpad_fields_json($type, $page_id) {
  $results = db_query('SELECT * FROM {pdf_forms_sigpad_data} WHERE bundle = :bundle AND page = :page',
  array(
    ':bundle' => $type,
    ':page' => $page_id
  ));
  $fields = array();
  foreach($results as $row) {
    $fields[] = $row;
  }
  print drupal_json_output(array('fields' => $fields ));
  exit();
}

/**
* This callback function updates the field data for a PDF page.
*/
function pdf_forms_sigpad_fields_save_json() {
  watchdog('devel', print_r($_POST, true));

  $field_data = $_POST['fields'];

  try {
    foreach ($field_data as $field) {
      $primary_key = isset($field->field) ? 'id' : array();
      drupal_write_record($field);
    }
    print drupal_json_output(array('status' => 1, 'message' => t('Field information saved.')));
  }
  catch (Exception $e) {
    watchdog_exception('pdf_forms_sigpad', $e);
    print drupal_json_output(array('status' => -1, 'message' => t('There was an error saving the field data.')));
  }
  exit();
}

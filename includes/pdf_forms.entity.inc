<?php

/**
* returns the title for a provided pdf_form_type.
*/
function pdf_forms_type_title($type) {
  $title = db_query('SELECT label FROM {pdf_forms_type} WHERE bundle = :bundle', array(':bundle' => $type))->fetchField();
  return $title;
}

/**
* Helper function to detect if a machine name exsits.
*/
function pdf_form_type_exists($machine_value) {
  return (db_query('SELECT COUNT(*) FROM {pdf_forms_type} WHERE bundle = :bundle', array(':bundle' => $machine_value))->fetchField() > 0);
}


/**
* PDF Form Type Creation.
*/
function pdf_forms_type_create() {
  $pdf_form_type = new stdClass();
  $pdf_form_type->bundle = '';
  $pdf_form_type->label = '';
  $pdf_form_type->description = '';
  $pdf_form_type->fid = 0;
  $pdf_form_type->status = 0;

  return drupal_get_form('pdf_forms_type_edit', $pdf_form_type);
}

/**
* PDF Form Type edit form.
*/
function pdf_forms_type_edit($form, &$form_state, $pdf_form_type = NULL) {
  if(!is_object($pdf_form_type)) {
    $results = db_query('SELECT * FROM {pdf_forms_type} WHERE bundle = :bundle', array(':bundle' => $pdf_form_type));
    foreach($results as $row) {
      $pdf_form_type = $row;
      break;
    }
  }

  $form['pdf_form_type'] = array(
    '#type' => 'value',
    '#value' => $pdf_form_type,
  );

  if($pdf_form_type->bundle == '') {
    $form['bundle'] = array(
      '#type' => 'machine_name',
      '#title' => t('Machine Name'),
      '#description' => t('The machine-readable name must contain only lowercase letters, numbers, and underscores.'),
      '#required' => TRUE,
      '#default_value' => $pdf_form_type->bundle,
      '#machine_name' => array(
        'exists' => 'pdf_form_type_exists'
      )
    );
  }

  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Enter the title users will see when submitting this form.'),
    '#required' => TRUE,
    '#default_value' => $pdf_form_type->label,
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('This will be displayed on the administrative pages only.'),
    '#default_value' => $pdf_form_type->description,
  );

  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Active'),
    '#default_value' => $pdf_form_type->status,
  );

  $form['file'] = array(
    '#type' => 'fieldset',
    '#title' => t('PDF Form'),
    '#prefix' => '<div id="pdf-upload-wrapper">',
    '#suffix' => '</div>'
  );

  $form['file']['fname_wrapper']['filename'] = array(
      '#type' => 'textfield',
      '#title' => t('Filename'),
      '#description' => t('Enter the name for the generated PDF file, you can use tokens in the file name. Do not include File extension.'),
      '#default_value' => (empty($pdf_form_type->filename) ? '' : $pdf_form_type->filename)
  );

  //@TODO: Have a checkbox which allows user to "enable" certain entity types which will then be added to $token_types
  $token_types = array('pdf_form', 'user');
  $form['file']['fname_wrapper']['tokens'] = array(
    '#theme' => 'token_tree',
    '#token_types' => $token_types,
  );

  $form['file']['fid'] = array(
    '#type' => 'managed_file',
    '#title' => t('File'),
    '#description' => t('Upload the PDF form to be filled out by submissions.'),
    '#required' => TRUE,
    '#upload_location' => variable_get('pdf_forms_template_path', 'public://pdf_form_templates'),
    '#field_name' => 'fid',
    '#default_value' => $pdf_form_type->fid
  );


  $form['file']['fid']['#ajax']['wrapper'] = 'pdf-upload-wrapper';

  // Display if FID is not empty or we just uploaded a file.
  if ( $pdf_form_type->fid > 0 || isset($form_state['values']['fid'])) {
    $form['fields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Field Mapping'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
    );

    // load up the file, then pass to our pdftk functions to extract fields.
    $file = ($pdf_form_type->fid > 0) ? file_load($pdf_form_type->fid) : file_load($form_state['values']['fid']);

    module_load_include('inc', 'pdf_forms', 'includes/pdf_forms.pdftk');
    if ($fields = pdf_forms_fields($file)) {
      $form['fields']['table'] = array(
        '#tree' => TRUE,
        '#theme' => 'table',
        '#header' => array(
          t('Field Name'), t('Field Type'), t('Field Prefix'), t('Field Value'), t('Field Suffix')
        ),
        '#rows' => array()
      );

      // load up any fields from the database that have been saved and update the default values with the stored values.
      $field_map_data = array();
      if (!empty($pdf_form_type->bundle)) {
        $query = db_select('pdf_forms_field_mapping', 'm');
        $query->fields('m', array())
          ->condition('m.bundle', $pdf_form_type->bundle, '=');
        $results = $query->execute();

        // Update the form's default values
        foreach($results as $row) {
          $field_map_data[$row->machine_name] =  array(
            'prefix' => $row->prefix,
            'suffix' => $row->suffix,
            'value' => $row->value
          );
        }
      }

      foreach($fields as $index => $field) {
          $id = str_replace(' ', '-', $field['name']);
          $id = trim($id);
          $field_name = array(
            '#id' => 'fields-table-' . $id . '-name',
            '#type' => 'markup',
            '#markup' => $field['name']
          );

          $field_type = array(
            '#id' => 'fields-table-' . $id . '-type',
            '#type' => 'markup',
            '#markup' => $field['type']
          );

          $prefix = array(
            '#id' => 'fields-table-' . $id . '-prefix',
            '#type' => 'textarea',
            '#default_value' => isset($field_map_data[$id]) ? $field_map_data[$id]['prefix'] : '',
          );

          $value = array(
            '#id' => 'fields-table-' . $id .  '-value',
            '#type' => 'textarea',
            '#default_value' => isset($field_map_data[$id]) ? $field_map_data[$id]['value'] : '',
          );

          $suffix = array(
            '#id' => 'fields-table-' . $id . '-suffix',
            '#type' => 'textarea',
            '#default_value' => isset($field_map_data[$id]) ? $field_map_data[$id]['suffix'] : '',
          );

          $machine_name = array(
            '#id' => 'fields-table-' . $id . '-machine-name',
            '#type' => 'value',
            '#value' => $field['name']
          );

          $form['fields']['table'][$id] = array(
            'name' => &$field_name,
            'type' => &$field_type,
            'prefix' => &$prefix,
            'value' => &$value,
            'suffix' => &$suffix,
            'machine_name' => &$machine_name,
          );

          $form['fields']['table']['#rows'][] = array(
            array('data' => &$field_name),
            array('data' => &$field_type),
            array('data' => &$prefix),
            array('data' => &$value),
            array('data' => &$suffix),
          );

          unset($field_name);
          unset($field_type);
          unset($prefix);
          unset($value);
          unset($suffix);
          unset($machine_name);
      }
      $token_types = array('pdf_form');
      $form['fields']['token_tree'] = array(
        '#theme' => 'token_tree',
        '#token_types' => $token_types,
      );



      // Include the Token information.

    }
    else {
      // notify user no fields were found in the form.
      $form['file']['fields']['table'] = array(
        '#type' => 'markup',
        '#markup' => t('No PDF Form fields were found in this PDF.')
      );
    }

  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
* Submit handler for the PDF Form Type form.
*/
function pdf_forms_type_edit_submit($form, &$form_state) {
  $values = $form_state['values'];
  $pdf_form_type = $values['pdf_form_type'];

  $new_bundle = FALSE;
  $pdf_form_type->bundle = isset($values['bundle']) ? $values['bundle'] : $pdf_form_type->bundle;
  $pdf_form_type->label = $values['label'];
  $pdf_form_type->status = $values['status'];
  $pdf_form_type->description = $values['description'];
  $pdf_form_type->filename = $values['filename'];
  $primary_keys = array ();

  // Let's deal with the file
  if (empty($pdf_form_type->fid) || $pdf_form_type->fid !== $values['fid']) {
    // The file has been changed
    if (empty($pdf_form_type->fid)) {
    // This is the first time this bundle has been saved
      $new_bundle = TRUE;
    }
    elseif ($pdf_form_type->fid !== $values['fid']) {
      // We are editing a bundle - the File ID has changed, which means we need to remove the old file + regenerate mappings
      // we need to delete mappings and update the fid.
      $old_file = file_load($pdf_form_type->fid);
      if ($old_file) {
        file_usage_delete($old_file, 'pdf_forms', $pdf_form_type->bundle, 0);
        file_delete($old_file);
        /* @TODO: Delete the mappings from pdf_form_field table */
      }
    }
    $file = file_load($values['fid']);
    file_usage_add($file, 'pdf_forms', $pdf_form_type->bundle, 0);
    $pdf_form_type->fid = $values['fid'];
  }
  // Save the bundle
  $primary_keys = isset($values['bundle']) ? array() : array('bundle');
  drupal_write_record('pdf_forms_type', $pdf_form_type, $primary_keys);

  /* Field mappings */
  $new_record = db_query('SELECT COUNT(*) FROM {pdf_forms_field_mapping} WHERE bundle = :bundle', array(':bundle' => $pdf_form_type->bundle))->fetchField();
 
  foreach($form_state['input']['fields']['table'] as $machine_name => $field) {
    $data = array(
      'bundle' => $pdf_form_type->bundle,
      'machine_name' =>   $machine_name,
      'name' =>   $machine_name,
      'prefix' => $field['prefix'],
      'value' =>  $field['value'],
      'suffix' => $field['suffix']
    );
    // Check if this is a new record
    $primary_keys = ($new_record > 0) ? array('bundle', 'machine_name') : array();
    $result = drupal_write_record('pdf_forms_field_mapping', $data, $primary_keys);
  }

  if ($new_bundle == TRUE ) {
    menu_rebuild();
    drupal_set_message(t("New PDF Form Created"));
    $form_state['redirect'] = "admin/content/pdf_forms/{$pdf_form_type->bundle}/edit";
  }
  else {
    drupal_set_message(t("PDF Form updates saved"));
  }
}



/**
* Implements the uri callback.
*/
function pdf_forms_uri($entity) {
  return array(
    'path' => "pdf_forms_submission/{$entity->submission_id}"
  );
}

/**
* Implements view callback.
*/
function pdf_forms_view($entity, $view_mode = 'full') {
  $entity_type = 'pdf_forms';
  $entity->content = array(
    '#view_mode' => $view_mode
  );
  // field_attach_prepare_view($entity_type, array($entity->submission_id => $entity), $view_mode);
  // entity_prepare_view($entity_type, array($entity->submission_id => $entity));
  // $entity->content += field_attach_view($entity_type, $entity, $view_mode);

  if (empty($entity->fid)) {
    module_load_include('inc', 'pdf_forms', 'includes/pdf_forms.pdftk');
    $file = pdftk_create_completed_form($entity);
    $entity->fid = $file->fid;
    pdf_form_save($entity);
  }
  $file = file_load($entity->fid);
  $entity->content['pdf'] = array(
    '#theme' => 'pdf_formatter_default',
    '#file' => $file,
    '#width' => '100%',
    '#height' => '500px'
  );

  // We are purposely not showing field api fields here.
  if (module_exists('rules')) {
    rules_invoke_event('pdf_forms_view', $entity, $file);
  }
  return $entity->content;
}

/**
* Callback for creating a new Form Submission.
*/
function pdf_forms_create($bundle) {
  $entity = entity_get_controller('pdf_form')->create($bundle);
  return drupal_get_form('pdf_forms_edit', $entity);
}

/**
* Form for editing/creating a form submission.
*/
function pdf_forms_edit($form, &$form_state, $entity = NULL) {

  $form['entity'] = array(
    '#type' => 'value',
    '#value' => $entity
  );

  field_attach_form('pdf_form', $entity, $form, $form_state);


  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 500
  );
  return $form;
}

function pdf_forms_edit_validate($form, &$form_state) {
  field_attach_form_validate('pdf_form', $form_state['values']['entity'], $form, $form_state);
}

function pdf_forms_edit_submit($form, &$form_state) {
  $entity = $form_state['values']['entity'];
  global $user;
  $entity->uid = $user->uid;
  field_attach_submit('pdf_form', $entity, $form, $form_state);
  $entity = pdf_form_save($entity);
  $form_state['redirect'] = "pdf_form_submission/{$entity->submission_id}";
}

/**
* Save by calling our controller.
*/
function pdf_form_save($entity) {
  return entity_get_controller('pdf_form')->save($entity);
}

<?php

/**
* Default Entity Controller for PDFForms Entity.
*/
class PDFFormsController extends DrupalDefaultEntityController {

  /**
  * Create a new Entity.
  */
  public function create($bundle) {
    $entity = new stdClass();
    $entity->bundle = $bundle;
    $entity->submission_id = 0;
    return $entity;
  }

  /**
  * Saves the entity.
  */
  public function save(&$entity) {
    module_load_include('inc', 'pdf_forms', 'includes/pdf_forms.pdftk');

    // We need to create the entity first, and then we need to generate the PDF file.
    if (empty($entity->submission_id)) {
      $entity->created = time();
    }
    module_invoke_all('entity_presave', $entity, 'pdf_form');

    if (module_exists('rules')) {
      $file = isset($entity->fid) ? file_load($entity->fid) : '';
      rules_invoke_event('pdf_forms_presave', $entity, $file);
    }

    $primary_keys = $entity->submission_id ? 'submission_id' : array();
    drupal_write_record('pdf_forms_data', $entity, $primary_keys);
    $invocation = 'entity_insert';
    if (empty($primary_keys)) {
      field_attach_insert('pdf_form', $entity);
    }
    else {
      $invocation = 'entity_update';
      field_attach_update('pdf_form', $entity);
    }
    module_invoke_all($invocation, $entity, 'pdf_form');

    // generate the pdf.
    $file = pdftk_create_completed_form($entity);
    $entity->fid = $file->fid;

    // Update the entity database record with the generated FID.
    $primary_keys = $entity->submission_id ? 'submission_id' : array();
    drupal_write_record('pdf_forms_data', $entity, $primary_keys);

    if (module_exists('rules')) {
      $file = isset($entity->fid) ? file_load($entity->fid) : '';

      switch($invocation) {
        case 'entity_insert':
          rules_invoke_event('pdf_forms_insert', $entity, $file);
        break;
        case 'entity_update':
          rules_invoke_event('pdf_forms_update', $entity, $file);
        break;
      }
    }

    cache_clear_all();
    return $entity;
  }

  public function delete($entity) {
    $this->deleteMultiple(array($entity));
  }

  public function deleteMultiple($entities) {
    $ids = array();
    if (!empty($entities)) {
      $transaction = db_transaction();
      try {
        foreach ($entities as $entity) {
          module_invoke_all('entity_delete', $entity, 'pdf_form');

          field_attach_delete('pdf_form', $entity);

          if (module_exists('rules')) {
            $file = isset($entity->fid) ? file_load($entity->fid) : '';
            rules_invoke_event('pdf_forms_delete', $entity, $file);
          }

          $ids[] = $entity->submission_id;
        }

        db_delete('pdf_forms_data')
          ->condition('submission_id', $ids, 'IN')
          ->execute();
      }
      catch (Exception $e) {
        $transaction->rollback();
        watchdog_exception('entity_example', $e);
        throw $e;
      }
    }
  }

}

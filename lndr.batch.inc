<?php

/**
 * @file
 * $Lndr batch functions.
 */

/**
 * Manually process all of the path syncing.
 *
 * @return mixed
 */
function lndr_sync() {
drush_print_r(__FUNCTION__);

  $nid = '';
  if (isset($_GET['nid'])) {
    // Sanitize $_GET['path'].
    $nid = check_plain($_GET['nid']);
    if (!is_numeric($nid)) {
      return MENU_ACCESS_DENIED;
    }
  }
  $function = 'lndr_sync_batch';
  $batch = $function($nid);
  batch_set($batch);

  return batch_process('');
}

/**
 * In Drupal 7, this is the a separate batch command where we
 * iterate through the number of operations.
 *
 * @param $nid
 *
 * @return array
 */
function lndr_sync_batch($nid) {
drush_print_r(__FUNCTION__);

  // @todo: This is not true batch yet as we only process one
  $ids = array(1);
  $num_operations = 1;
  drupal_set_message('Deploy lndr pages...');
  $operations = array();
  for ($i = 0; $i < $num_operations; $i++) {
    $operations[] = array(
      'lndr_sync_batch_op',
      array(
        $ids[$i],
        $nid,
      ),
    );
  }
  $batch = array(
    'operations' => $operations,
    'finished' => 'lndr_sync_batch_finished',
  );
  return $batch;
}

/**
 * The actual batch operation where we sync the path.
 *
 * @param $id
 * @param $path
 * @param $context
 */
function lndr_sync_batch_op($id, $nid, &$context) {
drush_print_r(__FUNCTION__);

  lndr_sync_path();
  // If we run this process with a $path passed in, it means it comes from a
  // Node that is reserved and without the lndr_project_id populated.
  if ($nid != '') {
    // We check after running the sync, if that node has an updated lndr_project_id.
    $node = node_load($nid);
    if (!empty($node->lndr_project_id)) {
      $context['results']['node_updated'] = $nid;
    }
  }
}

/**
 * Batch operation finishing callback.
 *
 * @param $success
 * @param $results
 * @param $operations
 */
function lndr_sync_batch_finished($success, $results, $operations) {
drush_print_r(__FUNCTION__);

  if ($success) {
    drupal_set_message(t('Finished deploying Lndr pages.'));
    // If we were sent from a placeholder (/lndr/reserved => path) but the page has been
    // published, we redirect back to that alias so user can see the published page.
    if (array_key_exists('node_updated', $results)) {
      drupal_goto('node/' . $results['node_updated']);
    }
    else {
      // If not, let's go home so we don't create an infinite loop.
      drupal_goto('<front>');
    }
  }
  else {
    $error_operation = reset($operations);
    drupal_set_message(
      t('An error occurred while processing @operation with arguments : @args',
        array(
          '@operation' => $error_operation[0],
          '@args' => print_r($error_operation[0], TRUE),
        )
      ),
      'error'
    );
  }
}

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
function lndr_sync($node) {
  $function = 'lndr_sync_batch';
  $batch = $function($node->nid);
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
  drupal_set_message('Deploy lndr pages...');
  $operations = array();
  module_load_include('inc', 'lndr');
  $alias = drupal_lookup_path('alias', "node/$nid", LANGUAGE_NONE);
  $project = lndr_find_project_by_url($alias);
  if ($project["published"] == TRUE) {
    $resource_files = lndr_import_resources($project);
    foreach ($resource_files as $resource_file) {
      $operations[] = array(
        'lndr_sync_batch_operation',
        array(
          $resource_file,
          $nid,
          $project,
        ),
      );
    }
  }
  $batch = array(
    'operations' => $operations,
    'finished' => 'lndr_sync_batch_finished',
    'file' => drupal_get_path('module', 'lndr') . '/lndr.batch.inc',
  );
  return $batch;
}

/**
 * Wrapper function for cron.
 *
 * @param $uri
 * @param $nid
 * @param $project
 */
function lndr_sync_source_refresh_callback($uri, $project) {
  // Last fifth element has full file path.
  $public_uri_array = explode('/', $uri, 5);
  // Last element of array is file name e.g. assets/img/image1.png.
  $target = end($public_uri_array);
  // Public directory e.g. public://lndr/[lndr_id].
  $directory = implode('/', array_slice($public_uri_array, 0, 4));
  lndr_save_unmanaged_files($target, $directory, $project['origin_url']);
}

/**
 * The actual batch operation where we sync the path and files.
 *
 * @param $uri
 * @param $nid
 * @param $project
 * @param $context
 */
function lndr_sync_batch_operation($uri, $nid, $project, &$context) {
  lndr_sync_source_refresh_callback($uri, $project);
  $context['results']["node_updated"] = [
    'nid' => $nid,
    'project_id' => $project['id'],
  ];
  return;
}

/**
 * Batch operation finishing callback.
 *
 * @param $success
 * @param $results
 * @param $operations
 *
 * @throws \Exception
 */
function lndr_sync_batch_finished($success, $results, $operations) {
  if ($success) {
    module_load_include('inc', 'lndr');
    drupal_set_message(t('Finished deploying Lndr pages.'));
    // If we were sent from a placeholder (/lndr/reserved => path) but the page has been
    // published, we redirect back to that alias so user can see the published page.
    if ($success && isset($results["node_updated"]) && is_numeric($results["node_updated"]['nid'])) {
      $node = node_load($results["node_updated"]['nid']);
      $project = lndr_find_project_by_prop('id', $results["node_updated"]['project_id']);
      $node = lndr_reserve_node(NULL, $project, $node);
      if ($node != FALSE) {
        drupal_goto('node/' . $results['node_updated']['nid']);
      }
    }
    // @TODO Throw some error.
    // If not, let's go home so we don't create an infinite loop.
    drupal_goto('<front>');
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

<?php
/**
 * @file
 * Code for the Lndr module
 */
/**
 * Implements hook_drush_command().
 */
function lndr_drush_command() {
drush_print_r(__FUNCTION__);


  $items['lndr'] = array(
    'description' => 'Demonstrate how Drush commands work.',
  );

  return $items;
}

/**
 */
function drush_lndr() {
drush_print_r(__FUNCTION__);
//
//  //  drush_print_r(lndr_find_project_by_url('admin2'));
//
//  $path = array(
//    'source' => '<front>',
//    'alias' => trim('it/does/not/matter', '/'),
//  );
//
//  // @see lndr_validate_path()
//  path_save($path);
//  $path['source'] = 'admin/config/search/path';
//  path_save($path);
  //  global $base_url;
  //  $projects = lndr_get_projects();
  //  $projects = lndr_get_projects();
  //
  //  drush_print_r($projects);
  //
  //  drush_print_r(lndr_load('vLyUY'));
  //lndr_source_refresh($args);
  //lndr_find_project_by_prop($prop, $value);
  //lndr_find_project_by_url($path);
  //lndr_load($alias);
  //lndr_file_transfer($node, $target);
  //lndr_source_file_deliver($node);
  //lndr_client_get_projects();
  //lndr_sync_path();
  //lndr_import_resources($url);
  //lndr_save_unmanaged_files($path, $directory, $url = NULL);
//   Remove the node type and all of associated nodes
    $type = 'lndr_page';
    // First, delete nodes
    $results = db_select('node', 'n')
      ->fields('n', array('nid'))
      ->condition('type', $type)
      ->execute();
    foreach ($results as $result) {
      $nids[] = $result->nid;
    }
    if (!empty($nids)) {
      node_delete_multiple($nids);
    }
  //  lndr_sync_path();
  exit;
  $time = new DateTime('2018-08-08T13:30:14.292Z');
  drush_print_r($time->getTimestamp());
  //  cache_clear_all('lndr_get_projects', 'cache');

  $result = lndr_get_projects();
  $publish_url = $result['projects'][0]['publish_url'];
  $_path = trim(substr($publish_url, strlen($base_url)), '/');
  //  lndr_reserve_node($_path);
  $alias = drupal_lookup_path('source', $_path, LANGUAGE_NONE);
  drush_print_r($_path);
  $object = menu_get_object('node', 1, $alias);

  drush_print_r($object->update < $time->getTimestamp());
  $queue = DrupalQueue::get('lndr_source_feed');
  foreach ($result['projects'] as $project) {
    //    $resource_file = lndr_save_unmanaged_files('stylesheets/styles.css', 'public://lndr/praesent', 'https://lndr.co/p/praesent/');
    //    drush_print_r($resource_file);
    $queue
      ->createItem(array($project, $object->nid, $object->changed));
  }

}

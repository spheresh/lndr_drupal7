<?php
/**
 * @file
 * ${NAME} class implementation.
 */

/**
 * Gets an directory by origin url.
 *
 * @param $project
 *    Lndr project object.
 *
 * @return array
 *    Reverse parsed url.
 */
function lndr_get_origin_directory($project) {
  $url_parts = explode('/', trim($project["origin_url"], '\/'));
  return array_reverse($url_parts);
}

/**
 * Gets an public url by project.
 *
 * @param $project
 *    Lndr project object.
 *
 * @return string
 *    Public drupal folder string.
 */
function lndr_get_public_by_project($project) {
  return 'public://lndr/' . $project['id'];
}

/**
 * Returns an array of all project from Lndr service.
 *
 * @return array
 */
function lndr_get_projects() {
  $result = &drupal_static(__FUNCTION__);
  if (!isset($result)) {
    $result = lndr_client_get_projects();
  }
  return $result;
}

/**
 * Implements hook_cron_queue_info().
 */
function lndr_cron_queue_info() {
  $queues['lndr_source_feed'] = [
    'worker callback' => 'lndr_source_refresh',
    'time' => 60,
  ];
  return $queues;
}

/**
 * Cron queue functional callback.
 */
function lndr_source_refresh($args) {
  list($project, $nid, $changed) = $args;
  // @TODO Performance Critical! Check node changed property and file updated property!.
  if ($project["published"] == TRUE) {
    lndr_import_resources($project['origin_url']);
  }
}

/**
 * Return project by property from projects array.
 *
 * @param $prop
 *    Property name.
 * @param $value
 *    The searched value.
 *
 * @return array|object|bool
 */
function lndr_find_project_by_prop($prop, $value) {
  $data = lndr_get_projects();
  $search_res = array_search($value, array_column(((array) $data)['projects'], $prop));
  return (FALSE !== $search_res) ? ((array) $data)['projects'][$search_res] : FALSE;
}

/**
 * Return project by url from projects array.
 *
 * @param $path
 *    The searched path.
 *
 * @return array|object|bool
 */
function lndr_find_project_by_url($path) {
  return lndr_find_project_by_prop('publish_url', url($path, ['absolute' => TRUE]));
}

/**
 * Implements hook_load().
 */
function lndr_load($alias) {

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', LNDR_CT_TYPE)
    ->fieldCondition('lndr_project_id', 'value', $alias, '=');
  $result = $query->execute();
  if (isset($result['node'])) {
    return node_load(reset($result['node'])->nid);
  }
  else {
    return FALSE;
  }
}

/**
 * Transfers a file to the client using HTTP.
 *
 * @param $node
 *    Lndr node object.
 *
 * @param $target
 *    File name.
 */
function lndr_file_transfer($node, $target) {
  $lang = $node->language;
  $lndr_project_id = $node->lndr_project_id[$lang][0]['value'];
  $project = lndr_find_project_by_prop('id', $lndr_project_id);

  $directory = lndr_get_public_by_project($project);
  if (!file_exists($image_uri = $directory . '/' . $target)) {
    lndr_save_unmanaged_files($target, $directory, $project["origin_url"] . '/');
  }
  $headers = file_download_headers($image_uri);
  // @TODO If headers are empty return MENU_ACCESS_DENIED.
  if (count($headers)) {
    foreach ($headers as $name => $value) {
      drupal_add_http_header($name, $value);
    }
  }
  file_transfer($image_uri, []);
}

/**
 * Menu callback function.
 *
 * @param $node
 *    Lndr node object.
 */
function lndr_source_file_deliver($node) {
  $args = func_get_args();
  array_shift($args);
  $target = implode('/', $args);
  lndr_file_transfer($node, $target);
}


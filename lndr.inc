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
drush_print_r(__FUNCTION__);

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
drush_print_r(__FUNCTION__);

  return 'public://lndr/' . $project['id'];
}

/**
 * Returns an array of all project from Lndr service.
 *
 * @return array
 */
function lndr_get_projects() {
drush_print_r(__FUNCTION__);
  $result = &drupal_static(__FUNCTION__);
  if (!isset($result)) {
    $result = lndr_client_get_projects();
  }
  return $result;
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
drush_print_r(__FUNCTION__);

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
drush_print_r(__FUNCTION__);

  return lndr_find_project_by_prop('publish_url', url($path, array('absolute' => TRUE)));
}

/**
 * Implements hook_load().
 */
function lndr_load($alias) {
drush_print_r(__FUNCTION__);


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
drush_print_r(__FUNCTION__);

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
drush_print_r(__FUNCTION__);

  $args = func_get_args();
  array_shift($args);
  $target = implode('/', $args);
  lndr_file_transfer($node, $target);
}

/**
 * Create an unpublished node to hold the URL path
 * string $path.
 *
 * @param string $path
 * @param string $lndr_id
 *
 * @return \stdClass
 * @throws \Exception
 */
function lndr_reserve_node($path, $lndr_id) {
drush_print_r(__FUNCTION__);

  $node = new stdClass();
  $node->title = $path;
  $node->type = LNDR_CT_TYPE;
  node_object_prepare($node);

  // @TODO These three properties will be overridden, but it cen be defined by default content type configuration.
  // @see node_object_prepare().
  // @file lndr.install It is not implemented yet.
  $node->uid = 0;
  $node->status = 1;
  $node->promote = 0;

  $node->language = LANGUAGE_NONE;
  $node->lndr_project_id[$node->language] = array(
    array('value' => $lndr_id),
  );

  $node->path['alias'] = $path;
  node_save($node);

  return $node;
}

/**
 * Checknig a path exists.
 *
 * @param string $path
 *
 * @return string
 *    Existing path.
 */
function lndr_check_path_exiting($path) {
drush_print_r(__FUNCTION__);

  return path_load(array('alias' => $path));
}

/**
 * Updating an existing alias.
 * @param int $nid
 * @param string $path
 *
 * @throws \Exception
 */
function lndr_update_node($nid, $path){
  $node_to_be_updated = node_load($nid);
  $node_to_be_updated->path['alias'] = $path;
  node_save($node_to_be_updated);
}

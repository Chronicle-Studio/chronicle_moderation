<?php

/**
 * @file
 * Provides display output for moderation pages.
 */

/**
 * Menu callback: display the moderation dashboard page.
 */
function chronicle_moderation_page() {
  $build = array();

  static $states;
  if (!isset($states)) {
    $states = array();
    foreach (workflow_get_workflow_states() as $state) {
      $states[$state->sid] = $state->state;
    }
  }

  $sql = 'SELECT n.title AS node_title,
      n.nid AS nid,
      n.language AS node_language,
      nr.timestamp AS nr_timestamp,
      nr.vid AS nr_vid,
      wn.stamp AS transition_stamp,
      wn.sid AS current_state,
      wnh.old_sid AS old_state
    FROM
      workflow_node wn
      LEFT JOIN workflow_node_history wnh ON (
        wn.nid = wnh.nid
        AND wn.stamp = wnh.stamp
      )
      LEFT JOIN node n ON wn.nid = n.nid
      LEFT JOIN node_revision nr ON n.vid = nr.vid
    WHERE wn.sid = :current_state
    ORDER BY transition_stamp DESC
  ';

  if (!$transitions = variable_get('chronicle_moderation_transitions', FALSE)) {
    return array(
      '#markup' => t('There are no workflow transitions configured.')
    );
  }

  foreach ($transitions as $key => $title) {
    $result = db_query($sql, array(':current_state' => $key));

    $rows = array();
    $header = array();
    foreach ($result as $item) {
      $item->current_state = $states[$item->current_state];
      $item->old_state = $states[$item->old_state];
      $rows[] = (array) $item;
    }
    $header = array_keys((array)$item);

    $build[$key] = array(
      '#prefix' => '<h2>' . $title . '</h2>',
      '#suffix' => '<br />',
      'list' => array(
        '#theme' => 'table',
        '#header' => $header,
        '#rows' => $rows,
      ),
    );
  }

  return $build;
}

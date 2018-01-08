<?php
/**
 * @class
 * Purpose: Provide a graphical summary of data stored in a materialized view.
 *   This is a generic, configurable fields to make it easier to add charts
 *   to Tripal Content pages.
 *
 * Data:
 *   Data is pulled from a configured materialized view and filtered based on a
 *   specific Tripal/Chado field.
 * Assumptions:
 *   - Currently there can only be one such chart per page. If you need more then
 *     one chart, a current work around is to extend this class using a different
 *     term.
 */
class local__single_series_chart extends ChadoChartField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Chado Single-Series Chart';

  // The default description for this field.
  public static $default_description = 'A type of graph in which all the data is part of a single series. Specifically, the dataset consists of labels and corresponding values.';

  // The default widget for this field.
  public static $default_widget = 'local__single_series_chart_widget';

  // The default formatter for this field.
  public static $default_formatter = 'local__single_series_chart_formatter';

}

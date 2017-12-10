# Tripal Fancy Fields

**UNDER ACTIVE DEVELOPMENT**

**NOT READY FOR USE**

I am currently working on implementing base classes to make development of advanced fields/formatters trivial. While the first release will require a very small amount of coding, future releases will have a UI to simplify creation of these fields even more!

## How To Use this Module
**Currently this module provides base classes only.**
1. Create a custom module. See the [Drupal Documentation](https://www.drupal.org/docs/7/creating-custom-modules) for how to do this.
2. Create your child class extending the base class of your choice in `[your module]/includes/TripalFields/[field name]/[field name].inc`
3. Set the various settings under `EDITABLE STATIC CONSTANTS`.
4. Implement hook_bundle_fields_info() and hook_bundle_instances_info() to attach your field to a Tripal Content Type.

**EXAMPLE:** See the local__chart field for an example field and includes/trpfancy_fields.fields.inc for example implementations of hook_bundle_fields_info() and hook_bundle_instances_info().

**NOTE:** Future development includes creation of a user interface allowing you to bypass the need to create a custom module.

## Chart Field
**UNDER DEVELOPMENT**

This field is designed to make adding charts to your Tripal Content Pages easy! The data is pulled from a materialized view of your choosing and filtered based on a specified field. For example, you could choose to add a pie chart to your organism pages showing the proportion of feature types for that organism. This would be done by creating a child class extending PieChartField and setting the `$default_instance_settings` as follows:
```php
    // The pie chart shown is able to be specific to the Drupal page by using a
    // Drupal/Tripal/Chado field to filter the materialized view. For example,
    // if you are on the "Tripalus databasica" organism page, then you may want
    // a pie chart showing the ratio/count of "Tripalus databasica" sequence
    // features. This is done using a combination of the filter_field (Drupal)
    // and filter_column (Materialized View).
    //-----------------------------------------------------
    // Use this setting to specify the Drupal/Tripal/Chado field containing the
    // value to filter the materialized view on. For example, if you want
    // `organism_feature_count.species = taxrank__species` then you set the
    // `filter_field = taxrank__species` and the `filter_column = species`.
    'filter_field' => 'taxrank__species',
    // Use this setting if the value you would like to filter on is the primary
    // key of the chado record being displayed on the page. For example, if you
    // want `organism_feature_count.organism_id = chado_record->organism_id`
    // then you set `use_record_id = TRUE` and `filter_column = organism_id`.
    'use_record_id' => FALSE,

    // The following relate to the materialized view:
    //-----------------------------------------------------
    // The table name of the materialized view to pull data from.
    'materialized_view' => 'organism_feature_count',
    // The column whose value should be the category labels on the chart.
    'label_column' => 'feature_type',
    // The column containing the numbers determining the pie piece size.
    'count_column' => 'num_features',
    // The column to filter the materialized view based on. This is usually
    // a foreign key but doesn't have to be.
    'filter_column' => 'species',
```

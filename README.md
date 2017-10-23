# Tripal Fancy Fields
This module provides additional fields to jazz up your Tripal Content Pages! These fields have been developed to be extremely configurable to allow you to customize them to your data without writing a single line of code!

## Chart Field
**UNDER DEVELOPMENT**

This field is designed to make adding charts to your Tripal Content Pages easy! The data is pulled from a materialized view of your choosing and filtered based on a specified field. For example, you could choose to add a pie chart to your organism pages showing the proportion of feature types for that organism. This would be done by selecting the existing organism_feature_count materialized view as the source of the data and specifying that the tripal obi__organism field maps to the organism_id column in the materialized view. Then just indicate which columns in the materialized view has the category labels and counts and you're ready to go!


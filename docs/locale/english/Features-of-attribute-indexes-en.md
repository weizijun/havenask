## Deduplication supported by multi-value attribute indexes



You can use in the schema.json file to deduplicate multi-value fields on which attribute indexes are created . Havenask hashes the values of the fields, and then stores the duplicate field values of multiple documents at the same location. This way, Havenask deduplicates multi-value attribute indexes by using shared storage space. The outcome of deduplication depends on the number of values in a field.



## Equal-value compression supported by single-value attribute indexes and multi-value attribute indexes



You can use the the schema.json file to configure equal-value compression based on  single-value fields or multi-value fields on which attribute indexes are created. Then, Havenask can use the equal-value compression feature to store N values of single-value index fields in n consecutive documents or N offset values that are generated by multi-value index fields in n consecutive documents. N is calculated by using the formula: N = 2 ^ n. If the values in the consecutive documents are identical, only one document is stored. If different values exist in the documents, the minimum value and deltas between the minimum value and the other values are stored to reduce storage usage. If equal-value compression delivers satisfactory performance, you can use this feature to compress the offset values of multi-value attribute indexes and save more storage.



## Updates of multi-value attribute indexes



By default, you cannot update a multi-value forward index. You can configure the parameters to allow a multi-value forward index to be updated. The working mechanisms of updating a multi-value forward index are different in online configurations and offline configurations.

In online configurations, after you configure the parameters to allow updates on the multi-value index field of a multi-value forward index, a dedicated scalable memory partition is created to store the updated values. The offsets of original values are also redirected to the memory partition. This way, real-time updates for a multi-value field can be implemented.

In offline configurations, after you configure the parameters to allow updates on the multi-value index field of a multi-value forward index, the updated values are stored in a patch file. Then, the system loads the patch file online and copies the file to a new location in a scalable memory partition. This way, you can read the updated values.

## Packing-based attribute storage



You can use packing-based attribute storage to pack a group of attributes of a document and store the attributes in a sequence of rows.

Compared with scenarios in which the attributes are stored in columns, the systemprovides better performance for a sequential read operation on attributes when the attributes are packed and then stored in a sequence of rows. This is because compact storage can reduce the number of cache misses in the CPU.

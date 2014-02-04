/*
// Boot Sector
// This is followed immediately by a NTFS spesific header
JMP						u8[3]
OEM System				u8[8]
Bytes Per Sector		u16
Sectors Per Cluster		u8
Reserved Sector Count	u16
Table Count				u8
Root Entry Count		u16
Sector Count			u16
Media Type				u8
Sectors Per Table		u16
Sectors Per Track		u16
Hidden Sector Count		u32
Sector Count (32-bit)	u32
Reserved				u32
Sector Count (64-bit)	u64


// Each record starts with the same header structure
Master File Table Cluster			u64
Master File Table Mirror Cluster	u64
Clusters Per Record					u8
Reserved							u8[3]
Clusters Per Index Buffer			u8
Reserved							u8[3]
Serial Number						u64
Checksum							u32

//Attributes have a variable length, but always start with the same sequence
Record Type					u8[4]
Update Sequence Offset		u16
Update Sequence Length		u16
Log File Sequence Number	u64
Record Sequence Number		u16
Hard Link Count				u16
Attributes Offset			u16
Flags						u16
Bytes In Use				u32
Bytes Allocated				u32
Parent Record Number		u64
Next Attribute Index		u32
Reserved					u32
Record Number				u64

// 0xffffffff marks the end of the attribute list
Attribute Type u32

//This length value defines the total length of the attribute record, including the "Attribute Type" and "Attribute Length" fields.
Attribute Length u32
*/
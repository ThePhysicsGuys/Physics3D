
# File Structure

## Part `76+?`
Important to note: the current CFrame of the part is not included, 
it is included by it's parent or calculated from the parent (using attachments and the CFrame of the parent for example)

Meaning              | Type           | Size (bytes)
-------------------- | -------------- | ------------
Shape of part        | Shape          | 28
Properties           | PartProperties | 48
User defined fields  | ...            | ?

### Shape `28`
Meaning        | Type   | Size (bytes)
-------------- | ------ | ------------
shape class ID | int    | 4
shape width    | double | 8
shape height   | double | 8
shape depth    | double | 8

### PartProperties `48`
Meaning        | Type   | Size (bytes)
-------------- | ------ | ------------
density        | double | 8
friction       | double | 8
bouncyness     | double | 8
conveyorEffect | Vec3   | 24

## PartWithCFrame
Meaning        | Type         | Size (bytes)
-------------- | ------------ | ------------
cframe of part | GlobalCFrame | 96
part           | Part         | 76+?

## MotorizedPhysical `192 + PhysicalSize`
Meaning | Type | Size (bytes)
--- | --- | ---
motion of global Center Of Mass | Motion       | 96
CFrame of main part             | GlobalCFrame | 96
Main Physical                   | Physical     | PhysicalSize

### Physical `8 + N * PartSize + (N-1) * 96 + sum(ConnectedPhysicalSize[i])`
Meaning                      | Type                 | Size (bytes)
---------------------------- | -------------------- | ------------
rigidBody of Physical        | RigidBody            | 4 + PartSize + (N-1) * (96 + PartSize)
number of attached physicals | unsigned int         | 4
List of Connected Physicals  | ConnectedPhysical[C] | sum(ConnectedPhysicalSize)

#### RigidBody `4 + PartSize + (N-1) * (96 + PartSize)`
N is the total Number of parts in the RigidBody

Meaning                  | Type              | Size (bytes)
------------------------ | ----------------- | ------------
Main Part                | Part*             | PartSize
number of attached parts | unsigned int      | 4
attached parts           | AttachedPart[N-1] | (N-1) * (96 + PartSize)

##### AttachedPart `96 + PartSize`
Meaning            | Type   | Size (bytes)
------------------ | ------ | ------------
Attachment to main | CFrame | 96
Attached Part      | Part*  | PartSize

### ConnectedPhysical `196 + ConstraintSize + PhysicalSize`
Meaning                      | Type           | Size (bytes)
---------------------------- | -------------- | ---
constraint attach to this    | CFrame         | 96
constraint attach to parent  | CFrame         | 96
constraint itself            | HardConstraint | 4 + ConstraintSize
information on this Physical | Physical       | PhysicalSize

#### HardConstraint `4 + ConstraintSize`
This is a dynamically serialized type

##### Builtin Hard Constraints
Identifier | Name | Extra Data | Size (bytes)
---------- | ---- | ---------- | ------------
0 | FixedConstraint | / | 0
1 | MotorConstraint | Vec3 angularVelocity <br> double currentAngle | 32


## World
Meaning                           | Type                             | Size (bytes)
--------------------------------- | -------------------------------- | ------------
number of serialized ShapeClasses | int                              | 4
shapeClasses                      | ShapeClass[ShapeClassCount]      | sum(ShapeClassSize)
number of Physicals               | size_t                           | 8
physicals                         | MotorizedPhysical[PhysicalCount] | sum(MotorizedPhysicalSize)
number of terrain Parts           | size_t                           | 8
terrain Parts                     | PartWithCFrame[TerrainPartCount] | TerrainPartCount * PartWithCFrameSize


### ShapeClass
This is a dynamically serialized type

#### Builtin Shape Classes
Identifier | Name | Extra Data | Size (bytes)
---------- | ---- | ---------- | ------------
0 | NormalizedPolyhedron | int vertexCount <br> int triangleCount <br> Vec3f[] vertices <br> Triangle[] triangles | 8 + vertexCount * 12 + triangleCount * 12

## A note on Dynamically serialized types:
Dynamically serializable types must be registered in a DynamicSerializerRegistry

When serialized using the dynamic serializer, the resulting byte stream will be an integer denoting the type, and then any information needed to deserialize the type. When deserializing, the dynamic deserializer reads the type int, and then hands off deserialization to the appropriate deserializer. 

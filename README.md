# Asset World System
The plugin is designed for clustering objects with further use and convenient tag search.  
The plugin is made for UE5  

## API Storage
|            API             | Description |
|:--------------------------:|-------------|
|    RegisterStorageAsset    | Register an object in the subsystem | 
| RegisterStorageAssetStatic |Register an object in the subsystem **STATIC**|
|      FindStorageAsset      | Search for a registered object in the subsystem | 
|   FindStorageAssetStatic   |Search for a registered object in the subsystem **STATIC**|
|    GetTypeStorageAsset     | Getting the type of storage | 
| GetTypeStorageAssetStatic  |Getting the type of storage **STATIC**|
|     RemoveStorageAsset     | Deleting an object from the storage | 
|  RemoveStorageAssetStatic  |Deleting an object from the storage **STATIC**|

![APIStorage Image](https://github.com/markveligod/AssetWorldSystem/raw/main/Resources/APIStorage.png)

## API Load Objects
|            API             | Description |
|:--------------------------:|-------------|
|    SyncObjectLoading    | Synchronous object loading |
|    SyncArrayObjectLoading    | Synchronous array object loading |
|    SyncClassLoading    | Synchronous class loading |
|    SyncArrayClassLoading    | Synchronous array class loading |
|    AsyncObjectLoading    | Asynchronous object loading |
|    AsyncObjectLoadingWithCallback    | Asynchronous object loading with callback delegate |
|    AsyncClassLoading    | Asynchronous class loading |
|    AsyncClassLoadingWithCallback    | Asynchronous class loading with callback delegate |

![APILoadObjects Image](https://github.com/markveligod/AssetWorldSystem/raw/main/Resources/APILoadObjects.png)

## Signatures
| Signatures | Description |
|:----------:|-------------|
|    OnAsyncLoadingObjectCompleted    | Notification of completion of asynchronous object loading |
|    OnAsyncLoadingClassCompleted    | Notification of completion of asynchronous UClass loading |
|    OnRegisterAssetCompleted    | Notification of registration of objects in the storage |
|    OnAssetRemoved    | Notification on deleting objects from storage |

![APILoadObjects Image](https://github.com/markveligod/AssetWorldSystem/raw/main/Resources/Signatures.png)


## 📫 Other
Attention: If you have changes, use PullRequest.

**Authors:**  
*[Mark Veligod](https://github.com/markveligod)* 

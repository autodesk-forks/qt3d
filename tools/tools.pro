TEMPLATE = subdirs

!qtHaveModule(3dcore): \
    return()

QT_FOR_CONFIG += 3dcore-private
qtConfig(qt3d-assimp):qtConfig(commandlineparser): {
    SUBDIRS += qgltf
}

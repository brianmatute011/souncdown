# Manual Release Guide

Guía paso a paso para crear un release en GitHub manualmente.

## Paso 1: Preparar el Código

```bash
# 1. Asegúrate de estar en la rama correcta
git checkout main  # o feature/soundcloud-cpp

# 2. Agrega todos los cambios
git add CHANGELOG.md PACKAGING.md README.md debian/

# 3. Commit
git commit -m "Release v1.0.0: Initial C++ version with progress bars"

# 4. Push a GitHub
git push origin main  # o tu rama
```

## Paso 2: Compilar el .deb

```bash
# Limpiar y compilar
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
cmake --build build -j$(nproc)

# Generar .deb
cd build
cpack

# Renombrar a formato estándar (si es necesario)
mv souncdown-1.0.0-Linux.deb souncdown_1.0.0_amd64.deb

# Verificar el paquete
ls -lh souncdown_1.0.0_amd64.deb
dpkg -I souncdown_1.0.0_amd64.deb

# Probar instalación local (opcional)
sudo apt install ./souncdown_1.0.0_amd64.deb
souncdown --version
sudo apt remove souncdown
```

## Paso 3: Crear Tag Local

```bash
# Volver al directorio raíz
cd ..

# Crear tag anotado con mensaje
git tag -a v1.0.0 -m "Release v1.0.0

Initial release of SounCdown C++ version

Features:
- Fast SoundCloud downloader with C++23
- Real-time progress bars with speed and ETA
- Playlist support with auto-numbering
- Multiple format conversion (MP3, FLAC, WAV, etc.)
- Metadata and thumbnail embedding
- No shell injection vulnerabilities

Technical:
- Modern C++23 implementation
- Memory-safe with RAII and smart pointers
- Modular architecture
- Comprehensive error handling
"

# Ver el tag
git tag -l -n9 v1.0.0

# Push el tag a GitHub
git push origin v1.0.0
```

## Paso 4: Crear Release en GitHub (Interfaz Web)

### Método A: Interfaz Web (Recomendado)

1. **Ve a tu repositorio en GitHub:**
   ```
   https://github.com/TU-USUARIO/souncdown
   ```

2. **Navega a Releases:**
   - Click en "Releases" (panel derecho)
   - O ve directamente a: `https://github.com/TU-USUARIO/souncdown/releases`

3. **Click en "Draft a new release"**

4. **Configurar el Release:**
   - **Tag version:** Selecciona `v1.0.0` (el tag que acabas de crear)
   - **Release title:** `v1.0.0 - Initial Release`
   - **Description:** Pega esto:

   ```markdown
   ## SounCdown v1.0.0 - Initial C++ Release

   Fast SoundCloud downloader with playlist support and real-time progress bars.

   ### Installation

   **Ubuntu/Debian:**
   ```bash
   wget https://github.com/TU-USUARIO/souncdown/releases/download/v1.0.0/souncdown_1.0.0_amd64.deb
   sudo apt install ./souncdown_1.0.0_amd64.deb
   ```

   **First use:**
   ```bash
   souncdown --check-deps
   souncdown --progress https://soundcloud.com/artist/track
   ```

   ### Features

   - ⚡ **Fast**: Native C++ implementation with C++23
   - 📊 **Progress Bars**: Real-time download progress with speed and ETA
   - 📋 **Playlists**: Download entire playlists with auto-numbering
   - 🎵 **Format Conversion**: MP3, FLAC, WAV, AAC, OGG, OPUS
   - 🎨 **Metadata**: Automatic thumbnail and metadata embedding
   - 🔒 **Secure**: No shell injection vulnerabilities

   ### What's New

   - Initial release of C++ version
   - Migrated from Python to modern C++23
   - Added interactive progress bars (like tqdm)
   - Improved performance and memory usage
   - Enhanced security with safe process execution
   - Modular architecture for maintainability

   ### Requirements

   - Ubuntu 20.04+ / Debian 11+
   - Dependencies: `yt-dlp`, `ffmpeg` (auto-installed)

   ### Documentation

   - [README.md](https://github.com/TU-USUARIO/souncdown/blob/main/README.md) - Quick start guide
   - [PACKAGING.md](https://github.com/TU-USUARIO/souncdown/blob/main/PACKAGING.md) - Build instructions
   - [CHANGELOG.md](https://github.com/TU-USUARIO/souncdown/blob/main/CHANGELOG.md) - Full changelog

   ### Support

   Report issues: https://github.com/TU-USUARIO/souncdown/issues
   ```

5. **Adjuntar el .deb:**
   - Arrastra y suelta `build/souncdown_1.0.0_amd64.deb` en la sección "Attach binaries"
   - O click en "Choose files" y selecciónalo

6. **Opciones:**
   - ☑️ **Set as the latest release** (marcado)
   - ☐ **Set as a pre-release** (desmarcado)
   - ☐ **Create a discussion** (opcional)

7. **Click "Publish release"**

### Método B: Usando gh CLI (Terminal)

Si tienes instalado `gh` (GitHub CLI):

```bash
# Instalar gh si no lo tienes
sudo apt install gh

# Autenticarse
gh auth login

# Crear release con el .deb adjunto
gh release create v1.0.0 \
  build/souncdown_1.0.0_amd64.deb \
  --title "v1.0.0 - Initial Release" \
  --notes "## SounCdown v1.0.0

Fast SoundCloud downloader with C++23

### Installation
\`\`\`bash
wget https://github.com/$(gh repo view --json nameWithOwner -q .nameWithOwner)/releases/download/v1.0.0/souncdown_1.0.0_amd64.deb
sudo apt install ./souncdown_1.0.0_amd64.deb
\`\`\`

### Features
- Real-time progress bars
- Playlist support
- Format conversion
- Metadata embedding

See [CHANGELOG.md](CHANGELOG.md) for full details."
```

## Paso 5: Verificar el Release

1. **Ve a tu release:**
   ```
   https://github.com/TU-USUARIO/souncdown/releases/tag/v1.0.0
   ```

2. **Verifica que:**
   - ✅ El tag `v1.0.0` existe
   - ✅ El archivo `souncdown_1.0.0_amd64.deb` está adjunto
   - ✅ La descripción se ve correcta
   - ✅ Está marcado como "Latest release"

3. **Prueba el link de descarga:**
   ```bash
   # Copia el link del .deb desde GitHub
   # Debería ser algo como:
   # https://github.com/TU-USUARIO/souncdown/releases/download/v1.0.0/souncdown_1.0.0_amd64.deb
   
   # Prueba descargarlo:
   wget https://github.com/TU-USUARIO/souncdown/releases/download/v1.0.0/souncdown_1.0.0_amd64.deb
   
   # Instálalo:
   sudo apt install ./souncdown_1.0.0_amd64.deb
   
   # Prueba que funcione:
   souncdown --version
   souncdown --check-deps
   ```

## Para Futuras Versiones

Cuando hagas cambios y quieras liberar v1.1.0:

```bash
# 1. Actualiza CHANGELOG.md con los cambios
# 2. Actualiza version en CMakeLists.txt si es necesario
# 3. Commit y push
git add CHANGELOG.md CMakeLists.txt
git commit -m "Bump version to v1.1.0"
git push

# 4. Compila nuevo .deb
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
cmake --build build -j$(nproc)
cd build && cpack
mv souncdown-1.1.0-Linux.deb souncdown_1.1.0_amd64.deb

# 5. Crea nuevo tag
cd ..
git tag -a v1.1.0 -m "Release v1.1.0: [Descripción de cambios]"
git push origin v1.1.0

# 6. Crea release en GitHub con el nuevo .deb
```

## Tips

### Renombrar Tag (si te equivocaste)

```bash
# Eliminar tag local
git tag -d v1.0.0

# Eliminar tag remoto
git push origin :refs/tags/v1.0.0

# Crear tag correcto
git tag -a v1.0.0 -m "Mensaje correcto"
git push origin v1.0.0
```

### Eliminar Release

1. Ve a: `https://github.com/TU-USUARIO/souncdown/releases`
2. Click en el release
3. Click en "Delete" (arriba a la derecha)
4. Confirma

### Editar Release

1. Ve al release
2. Click en "Edit release" (arriba a la derecha)
3. Modifica lo que necesites
4. "Update release"

## Checklist de Release

- [ ] Código commiteado y pusheado
- [ ] CHANGELOG.md actualizado
- [ ] Version correcta en CMakeLists.txt
- [ ] .deb compilado y testeado localmente
- [ ] Tag creado y pusheado
- [ ] Release creado en GitHub
- [ ] .deb adjunto al release
- [ ] README.md actualizado con instrucciones de instalación
- [ ] Release verificado y funcional
- [ ] Anunciado en redes sociales / comunidad (opcional)

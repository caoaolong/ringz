// ===== Core Editor Hook: Manages Leafer App, state, and all editor operations =====

import { useState, useRef, useEffect, useCallback } from 'react'
import {
  App,
  Rect,
  Ellipse,
  Path,
  Text,
  Star,
  type IUI,
} from 'leafer-ui'
import '@leafer-in/editor'
import '@leafer-in/viewport'
import '@leafer-in/state'
import '@leafer-in/export'
import { parseSVG, generateElementId } from '../utils/svgParser'
import { AnimationPlayer } from '../utils/animationEngine'
import { exportToSVG, downloadFile } from '../utils/svgExporter'
import type {
  LayerInfo,
  ElementProps,
  AnimationTrack,
  Keyframe,
} from '../types'

/** Extract editable properties from a Leafer element */
function extractProps(el: IUI): ElementProps {
  const fill = el.fill
  const stroke = el.stroke
  return {
    x: Math.round(el.x || 0),
    y: Math.round(el.y || 0),
    width: Math.round(el.width || 0),
    height: Math.round(el.height || 0),
    rotation: Math.round((el.rotation || 0) * 100) / 100,
    scaleX: Math.round((el.scaleX || 1) * 100) / 100,
    scaleY: Math.round((el.scaleY || 1) * 100) / 100,
    opacity: el.opacity ?? 1,
    fill: typeof fill === 'string' ? fill : '#4A90D9',
    stroke: typeof stroke === 'string' ? stroke : 'none',
    strokeWidth: typeof el.strokeWidth === 'number' ? el.strokeWidth : Number(el.strokeWidth) || 0,
    cornerRadius: typeof (el as any).cornerRadius === 'number' ? (el as any).cornerRadius : 0,
  }
}

/** Recursively track elements in the map with unique IDs */
function trackElement(el: IUI, map: Map<string, IUI>): void {
  const id = String(el.id || `auto_${el.innerId}`)
  if (!el.id) el.id = id
  map.set(id, el)
  const children = (el as any).children as IUI[] | undefined
  if (children) {
    for (const child of children) {
      trackElement(child, map)
    }
  }
}

/** Recursively build a flat layer list with depth info */
function buildLayerList(elements: IUI[], depth: number): LayerInfo[] {
  const layers: LayerInfo[] = []
  const arr = elements as IUI[]
  for (let i = arr.length - 1; i >= 0; i--) {
    const el = arr[i]
    const id = String(el.id || el.innerId)
    layers.push({
      id,
      name: (el as any).name || el.tag || 'Element',
      tag: el.tag || 'UI',
      visible: el.visible !== false,
      locked: (el as any).locked === true,
      opacity: el.opacity ?? 1,
      depth,
    })
    const children = (el as any).children as IUI[] | undefined
    if (children && children.length > 0) {
      layers.push(...buildLayerList(children, depth + 1))
    }
  }
  return layers
}

export function useEditor() {
  const containerRef = useRef<HTMLDivElement>(null)
  const appRef = useRef<App | null>(null)
  const elementMap = useRef<Map<string, IUI>>(new Map())
  const playerRef = useRef<AnimationPlayer | null>(null)
  const updateScheduled = useRef(false)

  const [isReady, setIsReady] = useState(false)
  const [selectedId, setSelectedId] = useState<string | null>(null)
  const [selectedProps, setSelectedProps] = useState<ElementProps | null>(null)
  const [layers, setLayers] = useState<LayerInfo[]>([])
  const [tracks, setTracks] = useState<Map<string, AnimationTrack>>(new Map())
  const [currentTime, setCurrentTime] = useState(0)
  const [duration, setDurationState] = useState(5)
  const [isPlaying, setIsPlaying] = useState(false)

  // ===== Initialize Leafer App =====
  useEffect(() => {
    if (!containerRef.current) return

    const app = new App({
      view: containerRef.current,
      editor: {
        boxSelect: true,
        hover: true,
      },
      tree: {},
      sky: {},
    })

    appRef.current = app

    // Set transparent canvas background so CSS grid shows through
    const tree = (app as any).tree
    if (tree) {
      tree.fill = 'transparent'
    }

    // Set up editor event listeners
    const editor = (app as any).editor

    editor.on('editor.select', () => {
      const element = editor.element
      if (element) {
        const id = String(element.id || element.innerId)
        setSelectedId(id)
        setSelectedProps(extractProps(element))
      } else {
        setSelectedId(null)
        setSelectedProps(null)
      }
    })

    editor.on('editor.update', () => {
      if (updateScheduled.current) return
      updateScheduled.current = true
      requestAnimationFrame(() => {
        updateScheduled.current = false
        const element = editor.element
        if (element) {
          setSelectedProps(extractProps(element))
        }
      })
    })

    // Create animation player
    playerRef.current = new AnimationPlayer((id) => elementMap.current.get(id))
    playerRef.current.setOnTimeUpdate(setCurrentTime)

    setIsReady(true)
    refreshLayers()

    return () => {
      playerRef.current?.pause()
      app.destroy()
      appRef.current = null
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [])

  // ===== Sync tracks to animation player =====
  useEffect(() => {
    if (!playerRef.current) return
    const trackMap = new Map<string, Keyframe[]>()
    for (const [id, track] of tracks) {
      trackMap.set(id, track.keyframes)
    }
    playerRef.current.setTracks(trackMap)
  }, [tracks])

  // ===== Helper: Refresh layer list =====
  const refreshLayers = useCallback(() => {
    const app = appRef.current
    if (!app) return
    const tree = (app as any).tree
    if (!tree) return
    const children = (tree.children as IUI[]) || []
    setLayers(buildLayerList(children, 0))
  }, [])

  // ===== Helper: Get canvas center =====
  const getCanvasCenter = useCallback(() => {
    const app = appRef.current as any
    if (!app) return { x: 400, y: 300 }
    const w = app.canvas?.width || containerRef.current?.clientWidth || 800
    const h = app.canvas?.height || containerRef.current?.clientHeight || 600
    return { x: w / 2, y: h / 2 }
  }, [])

  // ===== SVG Import =====
  const importSVGFile = useCallback(async (file: File) => {
    const text = await file.text()
    await importSVGString(text)
  }, [])

  const importSVGString = useCallback(
    async (svgString: string) => {
      const app = appRef.current as any
      if (!app) return

      const { group } = parseSVG(svgString)

      // Add each direct child to canvas for flat editing
      const children = (group.children as IUI[]) || []
      const toAdd = [...children]

      for (const child of toAdd) {
        child.remove()
        ;(child as any).set({ editable: true })
        app.tree.add(child)
        trackElement(child, elementMap.current)
      }

      // If no children (single element), add the group itself
      if (toAdd.length === 0) {
        app.tree.add(group)
        trackElement(group, elementMap.current)
      }

      refreshLayers()
    },
    [refreshLayers],
  )

  // ===== Add Shape =====
  const addShape = useCallback(
    (type: string) => {
      const app = appRef.current as any
      if (!app) return

      const center = getCanvasCenter()
      let element: IUI

      const fill = '#4A90D9'
      const stroke = '#2c5f8d'

      switch (type) {
        case 'Rect':
          element = new Rect({
            x: center.x - 60,
            y: center.y - 40,
            width: 120,
            height: 80,
            fill,
            stroke,
            strokeWidth: 2,
            cornerRadius: 8,
            editable: true,
            name: 'Rect',
          })
          break
        case 'Ellipse':
          element = new Ellipse({
            x: center.x - 50,
            y: center.y - 50,
            width: 100,
            height: 100,
            fill,
            stroke,
            strokeWidth: 2,
            editable: true,
            name: 'Ellipse',
          })
          break
        case 'Star':
          element = new Star({
            x: center.x - 50,
            y: center.y - 50,
            width: 100,
            height: 100,
            fill: '#F5A623',
            stroke: '#c97f0e',
            strokeWidth: 2,
            corners: 5,
            editable: true,
            name: 'Star',
          } as any)
          break
        case 'Path':
          element = new Path({
            path: 'M 0 0 L 50 0 L 25 50 Z',
            x: center.x - 25,
            y: center.y - 25,
            fill: '#7ED321',
            stroke: '#4a8b0c',
            strokeWidth: 2,
            editable: true,
            name: 'Path',
          } as any)
          break
        case 'Text':
          element = new Text({
            x: center.x - 60,
            y: center.y - 16,
            text: 'Hello',
            fontSize: 32,
            fill: '#333333',
            editable: true,
            name: 'Text',
          } as any)
          break
        default:
          return
      }

      const id = generateElementId()
      ;(element as any).id = id
      app.tree.add(element)
      elementMap.current.set(id, element)
      refreshLayers()

      // Select the new element
      app.editor?.select(element)
    },
    [getCanvasCenter, refreshLayers],
  )

  // ===== Delete Selected =====
  const deleteSelected = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    const element = app.editor?.element
    if (!element) return

    const id = String(element.id || element.innerId)

    // Remove from map
    elementMap.current.delete(id)

    // Remove animation track
    setTracks((prev) => {
      const next = new Map(prev)
      next.delete(id)
      return next
    })

    // Remove from canvas
    element.remove()
    app.editor?.select(null)

    setSelectedId(null)
    setSelectedProps(null)
    refreshLayers()
  }, [refreshLayers])

  // ===== Duplicate Selected =====
  const duplicateSelected = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    const element = app.editor?.element
    if (!element) return

    const clone = element.clone()
    clone.move(20, 20)
    const id = generateElementId()
    ;(clone as any).id = id
    ;(clone as any).set({ editable: true })
    app.tree.add(clone)
    elementMap.current.set(id, clone)
    refreshLayers()
    app.editor?.select(clone)
  }, [refreshLayers])

  // ===== Clear Canvas =====
  const clearCanvas = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    const tree = app.tree
    if (!tree) return

    const children = [...(tree.children as IUI[])]
    for (const child of children) {
      child.remove()
    }
    elementMap.current.clear()
    setTracks(new Map())
    setSelectedId(null)
    setSelectedProps(null)
    refreshLayers()
  }, [refreshLayers])

  // ===== Select Element =====
  const selectElement = useCallback((id: string) => {
    const app = appRef.current as any
    if (!app) return
    const element = elementMap.current.get(id)
    if (!element) return
    app.editor?.select(element)
    setSelectedId(id)
    setSelectedProps(extractProps(element))
  }, [])

  // ===== Deselect =====
  const deselectAll = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    app.editor?.select(null)
    setSelectedId(null)
    setSelectedProps(null)
  }, [])

  // ===== Update Property =====
  const updateProperty = useCallback(
    <K extends keyof ElementProps>(prop: K, value: ElementProps[K]) => {
      const app = appRef.current as any
      if (!app) return
      const element = app.editor?.element
      if (!element) return

      const data: any = {}
      if (prop === 'cornerRadius') {
        data.cornerRadius = value
      } else if (prop === 'fill') {
        data.fill = value === 'none' ? 'transparent' : value
      } else if (prop === 'stroke') {
        data.stroke = value === 'none' ? '' : value
      } else if (prop === 'strokeWidth') {
        data.strokeWidth = value
        if (value === 0) data.stroke = ''
      } else {
        data[prop] = value
      }

      element.set(data)

      // Update selected props without triggering editor.update loop
      setSelectedProps((prev) => (prev ? { ...prev, [prop]: value } : null))
    },
    [],
  )

  // ===== Toggle Layer Visibility =====
  const toggleLayerVisibility = useCallback(
    (id: string) => {
      const element = elementMap.current.get(id)
      if (!element) return
      element.visible = !element.visible
      refreshLayers()
    },
    [refreshLayers],
  )

  // ===== Toggle Layer Lock =====
  const toggleLayerLock = useCallback(
    (id: string) => {
      const element = elementMap.current.get(id)
      if (!element) return
      ;(element as any).locked = !(element as any).locked
      ;(element as any).hittable = !(element as any).locked
      refreshLayers()
    },
    [refreshLayers],
  )

  // ===== Rename Layer =====
  const renameLayer = useCallback(
    (id: string, name: string) => {
      const element = elementMap.current.get(id)
      if (!element) return
      ;(element as any).name = name
      refreshLayers()
    },
    [refreshLayers],
  )

  // ===== Animation: Add Keyframe =====
  const addKeyframe = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    const element = app.editor?.element
    if (!element) return

    const id = String(element.id || element.innerId)

    const keyframe: Keyframe = {
      id: generateElementId(),
      time: currentTime,
      props: {
        x: element.x,
        y: element.y,
        rotation: element.rotation,
        scaleX: element.scaleX,
        scaleY: element.scaleY,
        opacity: element.opacity ?? 1,
      },
      easing: 'linear',
    }

    setTracks((prev) => {
      const next = new Map(prev)
      const existing = next.get(id)
      if (existing) {
        const filtered = existing.keyframes.filter(
          (kf) => Math.abs(kf.time - currentTime) > 0.05,
        )
        next.set(id, {
          ...existing,
          keyframes: [...filtered, keyframe].sort((a, b) => a.time - b.time),
        })
      } else {
        next.set(id, {
          elementId: id,
          keyframes: [keyframe],
        })
      }
      return next
    })
  }, [currentTime])

  // ===== Animation: Remove Keyframe =====
  const removeKeyframe = useCallback((elementId: string, keyframeId: string) => {
    setTracks((prev) => {
      const next = new Map(prev)
      const track = next.get(elementId)
      if (track) {
        const filtered = track.keyframes.filter((kf) => kf.id !== keyframeId)
        if (filtered.length === 0) {
          next.delete(elementId)
        } else {
          next.set(elementId, { ...track, keyframes: filtered })
        }
      }
      return next
    })
  }, [])

  // ===== Animation: Play =====
  const playAnimation = useCallback(() => {
    const player = playerRef.current
    if (!player) return
    const app = appRef.current as any
    app?.editor?.select(null)
    player.setDuration(duration)
    player.play()
    setIsPlaying(true)
  }, [duration])

  // ===== Animation: Pause =====
  const pauseAnimation = useCallback(() => {
    playerRef.current?.pause()
    setIsPlaying(false)
  }, [])

  // ===== Animation: Stop =====
  const stopAnimation = useCallback(() => {
    playerRef.current?.stop()
    setIsPlaying(false)
    setCurrentTime(0)
  }, [])

  // ===== Animation: Seek =====
  const seekAnimation = useCallback((time: number) => {
    playerRef.current?.seek(time)
    setCurrentTime(time)
  }, [])

  // ===== Animation: Set Duration =====
  const setDuration = useCallback((d: number) => {
    setDurationState(d)
    playerRef.current?.setDuration(d)
  }, [])

  // ===== Export SVG =====
  const exportSVG = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    const children = (app.tree?.children as IUI[]) || []
    if (children.length === 0) return

    const center = getCanvasCenter()
    const svg = exportToSVG(children, center.x * 2, center.y * 2)
    downloadFile(svg, 'export.svg', 'image/svg+xml')
  }, [getCanvasCenter])

  // ===== Export PNG =====
  const exportPNG = useCallback(async () => {
    const app = appRef.current as any
    if (!app) return
    try {
      const result = await app.tree.export('png', { pixelRatio: 2 })
      if (result?.data) {
        const a = document.createElement('a')
        a.href = result.data
        a.download = 'export.png'
        a.click()
      }
    } catch (e) {
      console.error('PNG export failed:', e)
    }
  }, [])

  return {
    containerRef,
    isReady,
    selectedId,
    selectedProps,
    layers,
    tracks,
    currentTime,
    duration,
    isPlaying,
    importSVGFile,
    importSVGString,
    addShape,
    deleteSelected,
    duplicateSelected,
    clearCanvas,
    selectElement,
    deselectAll,
    updateProperty,
    toggleLayerVisibility,
    toggleLayerLock,
    renameLayer,
    addKeyframe,
    removeKeyframe,
    playAnimation,
    pauseAnimation,
    stopAnimation,
    seekAnimation,
    setDuration,
    exportSVG,
    exportPNG,
  }
}

export type UseEditorReturn = ReturnType<typeof useEditor>

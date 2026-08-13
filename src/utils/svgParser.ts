// ===== SVG Parser: Convert SVG XML to Leafer-UI elements =====

import {
  Rect,
  Ellipse,
  Line,
  Path,
  Polygon,
  Group,
  Text,
  type IUI,
  type IUIInputData,
} from 'leafer-ui'

let idCounter = 0
function genId(): string {
  return `el_${Date.now().toString(36)}_${(idCounter++).toString(36)}`
}

/** Parse an SVG string and return a Leafer Group containing all elements */
export function parseSVG(svgString: string): { group: Group; width: number; height: number } {
  const parser = new DOMParser()
  const doc = parser.parseFromString(svgString, 'image/svg+xml')

  // Check for parse errors
  const parseError = doc.querySelector('parsererror')
  if (parseError) {
    throw new Error('SVG 解析失败: XML 格式错误')
  }

  const svgEl = doc.documentElement
  if (!svgEl || svgEl.tagName.toLowerCase() !== 'svg') {
    throw new Error('未找到有效的 <svg> 根元素')
  }

  // Get viewBox and dimensions
  const viewBox = svgEl.getAttribute('viewBox')
  const widthAttr = svgEl.getAttribute('width')
  const heightAttr = svgEl.getAttribute('height')

  let vbX = 0,
    vbY = 0,
    vbW = 0,
    vbH = 0

  if (viewBox) {
    const parts = viewBox.split(/[\s,]+/).map(Number)
    if (parts.length === 4) {
      ;[vbX, vbY, vbW, vbH] = parts
    }
  }

  const width = vbW || parseFloat(widthAttr || '500') || 500
  const height = vbH || parseFloat(heightAttr || '400') || 400

  // Create root group
  const group = new Group({
    name: 'SVG Root',
    editable: true,
  })

  // Parse all child elements recursively
  const children = Array.from(svgEl.children)
  for (const child of children) {
    const element = parseSVGElement(child)
    if (element) {
      group.add(element)
    }
  }

  // Apply viewBox offset
  if (vbX !== 0 || vbY !== 0) {
    group.move(-vbX, -vbY)
  }

  return { group, width, height }
}

/** Recursively parse a single SVG element into a Leafer element */
function parseSVGElement(el: Element): IUI | null {
  const tag = el.tagName.toLowerCase()

  switch (tag) {
    case 'g':
      return parseGroup(el)
    case 'rect':
      return parseRect(el)
    case 'circle':
      return parseCircle(el)
    case 'ellipse':
      return parseEllipse(el)
    case 'line':
      return parseLine(el)
    case 'path':
      return parsePath(el)
    case 'polygon':
      return parsePolygon(el, true)
    case 'polyline':
      return parsePolygon(el, false)
    case 'text':
      return parseText(el)
    case 'defs':
    case 'style':
    case 'title':
    case 'desc':
    case 'metadata':
      return null
    default:
      return null
  }
}

function parseGroup(el: Element): Group {
  const group = new Group({
    name: el.getAttribute('id') || 'Group',
    editable: true,
  })

  applyCommonStyles(el, group)
  applyTransform(el, group)

  for (const child of Array.from(el.children)) {
    const element = parseSVGElement(child)
    if (element) {
      group.add(element)
    }
  }

  return group
}

function parseRect(el: Element): Rect {
  const x = parseFloat(el.getAttribute('x') || '0')
  const y = parseFloat(el.getAttribute('y') || '0')
  const width = parseFloat(el.getAttribute('width') || '100')
  const height = parseFloat(el.getAttribute('height') || '100')
  const rx = parseFloat(el.getAttribute('rx') || '0')

  const rect = new Rect({
    x,
    y,
    width,
    height,
    cornerRadius: rx || 0,
    editable: true,
    name: el.getAttribute('id') || 'Rect',
  })

  applyCommonStyles(el, rect)
  applyTransform(el, rect)
  return rect
}

function parseCircle(el: Element): Ellipse {
  const cx = parseFloat(el.getAttribute('cx') || '0')
  const cy = parseFloat(el.getAttribute('cy') || '0')
  const r = parseFloat(el.getAttribute('r') || '50')

  const ellipse = new Ellipse({
    x: cx - r,
    y: cy - r,
    width: r * 2,
    height: r * 2,
    editable: true,
    name: el.getAttribute('id') || 'Circle',
  })

  applyCommonStyles(el, ellipse)
  applyTransform(el, ellipse)
  return ellipse
}

function parseEllipse(el: Element): Ellipse {
  const cx = parseFloat(el.getAttribute('cx') || '0')
  const cy = parseFloat(el.getAttribute('cy') || '0')
  const rx = parseFloat(el.getAttribute('rx') || '50')
  const ry = parseFloat(el.getAttribute('ry') || '50')

  const ellipse = new Ellipse({
    x: cx - rx,
    y: cy - ry,
    width: rx * 2,
    height: ry * 2,
    editable: true,
    name: el.getAttribute('id') || 'Ellipse',
  })

  applyCommonStyles(el, ellipse)
  applyTransform(el, ellipse)
  return ellipse
}

function parseLine(el: Element): Line {
  const x1 = parseFloat(el.getAttribute('x1') || '0')
  const y1 = parseFloat(el.getAttribute('y1') || '0')
  const x2 = parseFloat(el.getAttribute('x2') || '100')
  const y2 = parseFloat(el.getAttribute('y2') || '100')

  const line = new Line({
    points: [x1, y1, x2, y2],
    editable: true,
    name: el.getAttribute('id') || 'Line',
  })

  applyCommonStyles(el, line)
  applyTransform(el, line)
  return line
}

function parsePath(el: Element): Path {
  const d = el.getAttribute('d') || ''

  const path = new Path({
    path: d,
    editable: true,
    name: el.getAttribute('id') || 'Path',
  } as IUIInputData)

  applyCommonStyles(el, path)
  applyTransform(el, path)
  return path
}

function parsePolygon(el: Element, closed: boolean): Polygon {
  const pointsStr = el.getAttribute('points') || ''
  const nums = pointsStr.trim().split(/[\s,]+/).map(Number)
  const points: number[] = []
  for (let i = 0; i < nums.length - 1; i += 2) {
    points.push(nums[i], nums[i + 1])
  }

  const polygon = new Polygon({
    points,
    editable: true,
    name: el.getAttribute('id') || (closed ? 'Polygon' : 'Polyline'),
  } as IUIInputData)

  applyCommonStyles(el, polygon)
  applyTransform(el, polygon)
  return polygon
}

function parseText(el: Element): Text {
  const x = parseFloat(el.getAttribute('x') || '0')
  const y = parseFloat(el.getAttribute('y') || '0')
  const fontSize = parseFloat(el.getAttribute('font-size') || '16')
  const text = el.textContent || ''

  const textEl = new Text({
    x,
    y,
    text,
    fontSize,
    editable: true,
    name: el.getAttribute('id') || 'Text',
  } as IUIInputData)

  applyCommonStyles(el, textEl)
  applyTransform(el, textEl)
  return textEl
}

/** Apply common SVG styles (fill, stroke, opacity) to a Leafer element */
function applyCommonStyles(el: Element, target: IUI): void {
  const fill = getComputedStyleAttr(el, 'fill')
  const stroke = getComputedStyleAttr(el, 'stroke')
  const strokeWidth = getComputedStyleAttr(el, 'stroke-width')
  const opacity = getComputedStyleAttr(el, 'opacity')
  const fillOpacity = getComputedStyleAttr(el, 'fill-opacity')
  const display = getComputedStyleAttr(el, 'display')
  const visibility = getComputedStyleAttr(el, 'visibility')

  const data: IUIInputData = {}

  if (fill && fill !== 'none') {
    data.fill = fill
  } else if (fill === 'none') {
    data.fill = 'transparent'
  }

  if (stroke && stroke !== 'none') {
    data.stroke = stroke
    if (strokeWidth) {
      data.strokeWidth = parseFloat(strokeWidth)
    }
  }

  if (opacity) {
    data.opacity = Math.max(0, Math.min(1, parseFloat(opacity)))
  }

  if (fillOpacity && fillOpacity !== '1') {
    // Approximate: multiply with opacity
    const baseOpacity = data.opacity ?? 1
    data.opacity = baseOpacity * parseFloat(fillOpacity)
  }

  if (display === 'none' || visibility === 'hidden') {
    data.visible = false
  }

  // Set a unique name if no id
  if (!target.name) {
    target.name = `el_${genId()}`
  }

  if (Object.keys(data).length > 0) {
    target.set(data)
  }
}

/** Get a computed style attribute from an SVG element */
function getComputedStyleAttr(el: Element, attr: string): string | null {
  // Direct attribute
  let val = el.getAttribute(attr)
  if (val) return val

  // Check inline style
  const style = el.getAttribute('style')
  if (style) {
    const match = style.match(new RegExp(`${attr}\\s*:\\s*([^;]+)`))
    if (match) return match[1].trim()
  }

  // Check parent inheritance for certain attributes
  const inherited = ['fill', 'stroke', 'stroke-width', 'opacity', 'font-size', 'display', 'visibility']
  if (inherited.includes(attr) && el.parentElement) {
    const parentTag = el.parentElement.tagName.toLowerCase()
    if (parentTag !== 'svg') {
      return getComputedStyleAttr(el.parentElement, attr)
    }
  }

  return null
}

/** Parse SVG transform attribute and apply to Leafer element */
function applyTransform(el: Element, target: IUI): void {
  const transform = el.getAttribute('transform')
  if (!transform) return

  const data: IUIInputData = {}

  // Parse translate
  const translateMatch = transform.match(/translate\(\s*([-\d.]+)\s*[,\s]\s*([-\d.]+)?\s*\)/)
  if (translateMatch) {
    data.x = (target.x || 0) + parseFloat(translateMatch[1])
    if (translateMatch[2]) {
      data.y = (target.y || 0) + parseFloat(translateMatch[2])
    }
  }

  // Parse rotate
  const rotateMatch = transform.match(/rotate\(\s*([-\d.]+)\s*(?:[,\s]\s*([-\d.]+)\s*[,\s]\s*([-\d.]+))?\s*\)/)
  if (rotateMatch) {
    const angle = parseFloat(rotateMatch[1])
    data.rotation = angle
    // Note: rotation with center point is complex; for simplicity we apply rotation around origin
  }

  // Parse scale
  const scaleMatch = transform.match(/scale\(\s*([-\d.]+)\s*(?:[,\s]\s*([-\d.]+))?\s*\)/)
  if (scaleMatch) {
    const sx = parseFloat(scaleMatch[1])
    const sy = scaleMatch[2] ? parseFloat(scaleMatch[2]) : sx
    data.scaleX = sx
    data.scaleY = sy
  }

  if (Object.keys(data).length > 0) {
    target.set(data)
  }
}

/** Generate a unique element ID */
export function generateElementId(): string {
  return genId()
}

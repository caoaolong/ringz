// ===== SVG Exporter: Convert Leafer elements to SVG string =====

import type { IUI } from 'leafer-ui'

/** Export an array of Leafer elements to an SVG string */
export function exportToSVG(elements: IUI[], width = 800, height = 600): string {
  const body = elements.map((el) => elementToSVG(el)).filter(Boolean).join('\n  ')
  return `<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg" width="${width}" height="${height}" viewBox="0 0 ${width} ${height}">
  ${body}
</svg>`
}

function elementToSVG(el: IUI, indent = '  '): string {
  const tag = el.tag || 'UI'
  const x = el.x || 0
  const y = el.y || 0
  const rotation = el.rotation || 0
  const opacity = el.opacity ?? 1
  const fill = typeof el.fill === 'string' ? el.fill : 'none'
  const stroke = typeof el.stroke === 'string' ? el.stroke : 'none'
  const strokeWidth = el.strokeWidth || 0
  const visible = el.visible !== false

  let styleAttrs = `fill="${fill}" stroke="${stroke}" stroke-width="${strokeWidth}" opacity="${opacity}"`
  if (!visible) styleAttrs += ' display="none"'

  let transform = ''
  if (x !== 0 || y !== 0) transform += `translate(${x}, ${y}) `
  if (rotation !== 0) transform += `rotate(${rotation}) `

  switch (tag) {
    case 'Rect': {
      const w = el.width || 100
      const h = el.height || 100
      const rx = (el as any).cornerRadius || 0
      const rxAttr = rx ? ` rx="${rx}"` : ''
      return `${indent}<rect x="0" y="0" width="${w}" height="${h}"${rxAttr} ${styleAttrs}${transform ? ` transform="${transform.trim()}"` : ''}/>`
    }
    case 'Ellipse': {
      const w = el.width || 100
      const h = el.height || 100
      const rx = w / 2
      const ry = h / 2
      return `${indent}<ellipse cx="${rx}" cy="${ry}" rx="${rx}" ry="${ry}" ${styleAttrs}${transform ? ` transform="${transform.trim()}"` : ''}/>`
    }
    case 'Line': {
      const points = (el as any).points || []
      if (points.length >= 4) {
        return `${indent}<line x1="${points[0]}" y1="${points[1]}" x2="${points[2]}" y2="${points[3]}" ${styleAttrs}${transform ? ` transform="${transform.trim()}"` : ''}/>`
      }
      return ''
    }
    case 'Path': {
      const d = (el as any).path || (el as any).__path || ''
      return `${indent}<path d="${d}" ${styleAttrs}${transform ? ` transform="${transform.trim()}"` : ''}/>`
    }
    case 'Polygon': {
      const points = (el as any).points || []
      const pointsStr = points.map((p: any) => (typeof p === 'number' ? p : `${p.x},${p.y}`)).join(' ')
      return `${indent}<polygon points="${pointsStr}" ${styleAttrs}${transform ? ` transform="${transform.trim()}"` : ''}/>`
    }
    case 'Text': {
      const text = (el as any).text || ''
      const fontSize = (el as any).fontSize || 16
      return `${indent}<text x="0" y="${fontSize}" font-size="${fontSize}" ${styleAttrs}${transform ? ` transform="${transform.trim()}"` : ''}>${escapeXml(text)}</text>`
    }
    case 'Star': {
      const w = el.width || 100
      const h = el.height || 100
      const cx = w / 2
      const cy = h / 2
      const outerR = Math.min(w, h) / 2
      const innerR = outerR * 0.4
      const corners = (el as any).corners || 5
      const points = starPoints(cx, cy, outerR, innerR, corners)
      return `${indent}<polygon points="${points}" ${styleAttrs}${transform ? ` transform="${transform.trim()}"` : ''}/>`
    }
    case 'Group': {
      const children = (el as any).children as IUI[] | undefined
      if (!children || children.length === 0) return ''
      const childSvg = children.map((c) => elementToSVG(c, indent + '  ')).filter(Boolean).join('\n')
      return `${indent}<g ${styleAttrs}${transform ? ` transform="${transform.trim()}"` : ''}>\n${childSvg}\n${indent}</g>`
    }
    default:
      return ''
  }
}

function starPoints(cx: number, cy: number, outerR: number, innerR: number, corners: number): string {
  const points: string[] = []
  const step = Math.PI / corners
  for (let i = 0; i < corners * 2; i++) {
    const r = i % 2 === 0 ? outerR : innerR
    const angle = i * step - Math.PI / 2
    points.push(`${(cx + r * Math.cos(angle)).toFixed(2)},${(cy + r * Math.sin(angle)).toFixed(2)}`)
  }
  return points.join(' ')
}

function escapeXml(text: string): string {
  return text
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&apos;')
}

/** Download a string as a file */
export function downloadFile(content: string, filename: string, mimeType = 'image/svg+xml'): void {
  const blob = new Blob([content], { type: mimeType })
  const url = URL.createObjectURL(blob)
  const a = document.createElement('a')
  a.href = url
  a.download = filename
  document.body.appendChild(a)
  a.click()
  document.body.removeChild(a)
  URL.revokeObjectURL(url)
}
